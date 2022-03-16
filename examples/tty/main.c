#include "../../hc/hc.h"
#include "../../hc/libc.h"
#include "../../hc/libc/musl.c"
#include "../../hc/wrappers.c"
#include "../../hc/libhc/util.c"
#include "../../hc/libhc/debug.c"
#include "../../hc/libhc/drmKms.c"

static int32_t init_graphics(struct drmKms *graphics) {
    // Set up frame buffer.
    int32_t status = drmKms_init(graphics, "/dev/dri/card0");
    if (status < 0) {
        debug_printNum("Failed to set up frame buffer (", status, ")\n");
        return -1;
    }
    // Print all available modes.
    for (uint32_t i = 0; i < graphics->connector.count_modes; ++i) {
        debug_printStr("Mode \"", graphics->modeInfos[i].name, "\"\n", DRM_DISPLAY_MODE_LEN);
        debug_printNum("  Pixel clock: ", graphics->modeInfos[i].clock, " Khz\n");
        debug_printNum("  Width: ", graphics->modeInfos[i].hdisplay, " pixels\n");
        debug_printNum("  Height: ", graphics->modeInfos[i].vdisplay, " pixels\n");
        debug_printNum("  Hsync: start=", graphics->modeInfos[i].hsync_start, ", ");
        debug_printNum("end=", graphics->modeInfos[i].hsync_end, ", ");
        debug_printNum("total=", graphics->modeInfos[i].htotal, "\n");
        debug_printNum("  Vsync: start=", graphics->modeInfos[i].vsync_start, ", ");
        debug_printNum("end=", graphics->modeInfos[i].vsync_end, ", ");
        debug_printNum("total=", graphics->modeInfos[i].vtotal, "\n");
        debug_printNum("  Refresh rate: ", graphics->modeInfos[i].vrefresh, " hz\n\n");
    }

    return 0;
}

static inline void deinit_graphics(struct drmKms *graphics) {
    drmKms_deinit(graphics);
}

int32_t main(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    // Argument parsing.
    if (argc != 2) {
        static const char usageStart[] = "Usage: ";
        static const char usageArgs[] = " TTY_NUM\n";
        hc_write(STDOUT_FILENO, &usageStart, sizeof(usageStart) - 1);
        if (argv[0] != NULL) hc_write(STDOUT_FILENO, argv[0], util_cstrLen(argv[0]));
        hc_write(STDOUT_FILENO, &usageArgs, sizeof(usageArgs) - 1);
        return 0;
    }

    // Parse TTY_NUM argument.
    uint64_t ttyNumber;
    if (util_strToUint(argv[1], '\0', &ttyNumber) == 0 || ttyNumber < 1 || ttyNumber > 63) {
        static const char error[] = "Invalid TTY_NUM argument\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return 1;
    }
    char ttyPath[10] = "/dev/tty\0\0";
    ttyPath[sizeof(ttyPath) - 2] = argv[1][0];
    if (ttyNumber > 9) {
       ttyPath[sizeof(ttyPath) - 1] = argv[1][1];
    }

    // Continue in a child process, to make sure setsid() will work.
    struct clone_args args = { .flags = CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_CLEAR_SIGHAND };
    int32_t status = hc_clone_exit(&args, sizeof(args));
    if (status < 0) return 1;

    // Create new session.
    status = hc_setsid();
    if (status < 0) return 1;

    // Open tty.
    int32_t ttyFd = hc_openat(-1, &ttyPath, O_RDWR, 0);
    if (ttyFd < 0) {
        static const char error[] = "Failed to open tty\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return 1;
    }

    // Set the tty as our controlling terminal.
    status = hc_ioctl(ttyFd, TIOCSCTTY, 0);
    if (status < 0) {
        static const char error[] = "Failed to set controlling terminal\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return 1;
    }

    // Set up signalfd for SIGUSR1 and SIGUSR2.
    uint64_t ttySignals = hc_SIGMASK(SIGUSR1) | hc_SIGMASK(SIGUSR2);
    status = hc_rt_sigprocmask(SIG_BLOCK, &ttySignals, NULL);
    if (status < 0) return 1;

    int32_t signalFd = hc_signalfd4(-1, &ttySignals, 0);
    if (signalFd < 0) return 1;

    // Request SIGUSR1 and SIGUSR2 when our tty is entered and left.
    struct vt_mode vtMode = {
        .mode = VT_PROCESS,
        .acqsig = SIGUSR1,
        .relsig = SIGUSR2
    };
    status = hc_ioctl(ttyFd, VT_SETMODE, &vtMode);
    if (status < 0) return 1;

    // Set tty to graphics mode.
    status = hc_ioctl(ttyFd, KDSETMODE, (void *)KD_GRAPHICS);
    if (status < 0) return 1;

    // Check if our tty is already active.
    struct vt_stat vtState;
    status = hc_ioctl(ttyFd, VT_GETSTATE, &vtState);
    if (status < 0) return 1;

    bool active = vtState.v_active == ttyNumber;
    struct drmKms graphics;
    if (active) {
        if (init_graphics(&graphics) < 0) return 1;
    }

    // Set up epoll.
    int32_t epollFd = hc_epoll_create1(0);
    if (epollFd < 0) return 1;

    struct epoll_event signalFdEvent = {
        .events = EPOLLIN
    };
    if (hc_epoll_ctl(epollFd, EPOLL_CTL_ADD, signalFd, &signalFdEvent) < 0) return 1;

    int64_t frameCounter;
    struct timespec prev;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    for (;;) {
        int32_t timeout = active ? 0 : -1; // Only block if not active.
        struct epoll_event event;
        status = hc_epoll_pwait(epollFd, &event, 1, timeout, NULL);
        if (status < 0) return 1;

        if (status > 0) {
            // Handle event.
            struct signalfd_siginfo info;
            int64_t numRead = hc_read(signalFd, &info, sizeof(info));
            if (numRead != sizeof(info)) return 1;

            if (info.ssi_signo == SIGUSR1) {
                if (active) return 1;
                active = true;
                static const char message[] = "Acquired!\n";
                hc_write(STDOUT_FILENO, &message, sizeof(message) - 1);

                if (init_graphics(&graphics) < 0) return 1;

                // Initialise drawing state.
                red = 0;
                green = 0;
                blue = 0;
                frameCounter = 0;
                hc_clock_gettime(CLOCK_MONOTONIC, &prev);
            } else if (info.ssi_signo == SIGUSR2) {
                if (!active) return 1;

                deinit_graphics(&graphics);
                status = hc_ioctl(ttyFd, VT_RELDISP, (void *)1);
                if (status < 0) return 1;

                active = false;
                static const char message[] = "Released!\n";
                hc_write(STDOUT_FILENO, &message, sizeof(message) - 1);
                continue; // Skip drawing.
            }
        }

        // Do drawing.
        uint32_t colour = (red << 16) | (green << 8) | blue;
        int32_t numPixels = (int32_t)(graphics.frameBufferSize >> 2);
        for (int32_t i = 0; i < numPixels; ++i) graphics.frameBuffer[i] = colour;
        drmKms_markFbDirty(&graphics);

        ++frameCounter;
        struct timespec now;
        hc_clock_gettime(CLOCK_MONOTONIC, &now);
        if (now.tv_sec > prev.tv_sec) {
            debug_printNum("FPS: ", frameCounter, "\n");
            frameCounter = 0;
            prev = now;
        }
        // Continuous iteration of colours.
        if (red == 0 && green == 0 && blue != 255) ++blue;
        else if (red == 0 && green != 255 && blue == 255) ++green;
        else if (red == 0 && green == 255 && blue != 0) --blue;
        else if (red != 255 && green == 255 && blue == 0) ++red;
        else if (red == 255 && green == 255 && blue != 255) ++blue;
        else if (red == 255 && green != 0 && blue == 255) --green;
        else if (red == 255 && green == 0 && blue != 0) --blue;
        else --red;
    }

    return 0;
}
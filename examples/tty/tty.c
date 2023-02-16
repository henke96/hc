#include "hc/hc.h"
#include "hc/util.c"
#include "hc/libc/musl.c"
#include "hc/linux/linux.h"
#include "hc/linux/util.c"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/drmKms.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone3_exit.c"

static int32_t init_graphics(struct drmKms *graphics) {
    // Set up frame buffer.
    int32_t status = drmKms_init(graphics, "/dev/dri/card0");
    if (status < 0) {
        debug_printNum("Failed to initialise graphics (", status, ")\n");
        return -1;
    }

    // Find the best display mode, prioritising resolution and refresh rate.
    int32_t selectedModeIndex = -1;
    int32_t selectedModeWidth = 0;
    int32_t selectedModeHz = 0;
    for (int32_t i = 0; i < graphics->connector.count_modes; ++i) {
        struct iovec print[] = {
            { hc_STR_COMMA_LEN("Mode \"") },
            { graphics->modeInfos[i].name, DRM_DISPLAY_MODE_LEN }
        };
        sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print)); 
        debug_printNum("\"\n  Pixel clock: ", graphics->modeInfos[i].clock, " KHz\n");
        debug_printNum("  Width: ", graphics->modeInfos[i].hdisplay, " pixels\n");
        debug_printNum("  Height: ", graphics->modeInfos[i].vdisplay, " pixels\n");
        debug_printNum("  Hsync: start=", graphics->modeInfos[i].hsync_start, ", ");
        debug_printNum("end=", graphics->modeInfos[i].hsync_end, ", ");
        debug_printNum("total=", graphics->modeInfos[i].htotal, "\n");
        debug_printNum("  Vsync: start=", graphics->modeInfos[i].vsync_start, ", ");
        debug_printNum("end=", graphics->modeInfos[i].vsync_end, ", ");
        debug_printNum("total=", graphics->modeInfos[i].vtotal, "\n");
        debug_printNum("  Refresh rate: ", graphics->modeInfos[i].vrefresh, " Hz\n");

        if (
            graphics->modeInfos[i].hdisplay > selectedModeWidth ||
            (graphics->modeInfos[i].hdisplay == selectedModeWidth && graphics->modeInfos[i].vrefresh > selectedModeHz)
        ) {
            selectedModeIndex = i;
            selectedModeWidth = graphics->modeInfos[i].hdisplay;
            selectedModeHz = graphics->modeInfos[i].vrefresh;
        }
    }
    if (selectedModeIndex < 0) return -1;

    struct iovec print[] = {
        { hc_STR_COMMA_LEN("Selected mode \"") },
        { graphics->modeInfos[selectedModeIndex].name, DRM_DISPLAY_MODE_LEN }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
    debug_printNum("\" at ", selectedModeHz, " Hz.\n");

    // Setup framebuffer using the selected mode.
    status = drmKms_setupFb(graphics, selectedModeIndex);
    if (status < 0) {
        debug_printNum("Failed to setup framebuffer (", status, ")\n");
        return -1;
    }

    return 0;
}

static inline void deinit_graphics(struct drmKms *graphics) {
    drmKms_deinit(graphics);
}

int32_t start(int32_t argc, char **argv) {
    // Set up epoll.
    int32_t epollFd = sys_epoll_create1(0);
    if (epollFd < 0) return 1;

    uint64_t ttyNumber;
    int32_t ttyFd;
    bool active = true;
    if (argc == 2) {
        // Parse TTY_NUM argument.
        if (util_strToUint(argv[1], 100, &ttyNumber) <= 0 || ttyNumber < 1 || ttyNumber > 63) {
            sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Invalid tty argument\n"));
            return 1;
        }
        char ttyPath[10] = "/dev/tty\0\0";
        ttyPath[sizeof(ttyPath) - 2] = argv[1][0];
        if (ttyNumber > 9) {
        ttyPath[sizeof(ttyPath) - 1] = argv[1][1];
        }

        // Continue in a child process, to make sure setsid() will work.
        struct clone_args args = { .flags = CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_CLEAR_SIGHAND };
        int32_t status = sys_clone3_exit(&args, sizeof(args));
        if (status < 0) return 1;

        // Create new session.
        status = sys_setsid();
        if (status < 0) return 1;

        // Open tty.
        ttyFd = sys_openat(-1, &ttyPath, O_RDWR, 0);
        if (ttyFd < 0) {
            sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Failed to open tty\n"));
            return 1;
        }

        // Set the tty as our controlling terminal.
        status = sys_ioctl(ttyFd, TIOCSCTTY, 0);
        if (status < 0) {
            sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Failed to set controlling terminal\n"));
            return 1;
        }

        // Set up signalfd for SIGUSR1 and SIGUSR2.
        uint64_t ttySignals = sys_SIGMASK(SIGUSR1) | sys_SIGMASK(SIGUSR2);
        status = sys_rt_sigprocmask(SIG_BLOCK, &ttySignals, NULL);
        if (status < 0) return 1;

        int32_t signalFd = sys_signalfd4(-1, &ttySignals, 0);
        if (signalFd < 0) return 1;

        // Request SIGUSR1 and SIGUSR2 when our tty is entered and left.
        struct vt_mode vtMode = {
            .mode = VT_PROCESS,
            .acqsig = SIGUSR1,
            .relsig = SIGUSR2
        };
        status = sys_ioctl(ttyFd, VT_SETMODE, &vtMode);
        if (status < 0) return 1;

        // Set tty to graphics mode.
        status = sys_ioctl(ttyFd, KDSETMODE, (void *)KD_GRAPHICS);
        if (status < 0) return 1;

        // Check if our tty is already active.
        struct vt_stat vtState;
        vtState.v_active = 0;
        status = sys_ioctl(ttyFd, VT_GETSTATE, &vtState);
        if (status < 0) return 1;
        active = vtState.v_active == ttyNumber;

        struct epoll_event signalFdEvent = {
            .events = EPOLLIN,
            .data.fd = signalFd
        };
        if (sys_epoll_ctl(epollFd, EPOLL_CTL_ADD, signalFd, &signalFdEvent) < 0) return 1;
    }

    struct drmKms graphics;
    int64_t frameCounter;
    struct timespec prev = {0};
    uint32_t red;
    uint32_t green;
    uint32_t blue;

    if (active) {
        // Initialise drawing state.
        if (init_graphics(&graphics) < 0) return 1;
        red = 0;
        green = 0;
        blue = 0;
        frameCounter = 0;
        debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &prev), RES == 0);
    }

    for (;;) {
        int32_t timeout = active ? 0 : -1; // Only block if not active.
        struct epoll_event event;
        event.data.fd = 0;
        int32_t status = sys_epoll_pwait(epollFd, &event, 1, timeout, NULL);
        if (status < 0) return 1;
        if (status > 0) {
            // Handle event.
            struct signalfd_siginfo info;
            info.ssi_signo = 0;
            int64_t numRead = sys_read(event.data.fd, &info, sizeof(info));
            if (numRead != sizeof(info)) return 1;

            if (info.ssi_signo == SIGUSR1) {
                if (active) return 1;
                active = true;
                sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Acquired!\n"));

                // Initialise drawing state.
                if (init_graphics(&graphics) < 0) return 1;
                red = 0;
                green = 0;
                blue = 0;
                frameCounter = 0;
                debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &prev), RES == 0);
            } else if (info.ssi_signo == SIGUSR2) {
                if (!active) return 1;

                deinit_graphics(&graphics);
                status = sys_ioctl(ttyFd, VT_RELDISP, (void *)1);
                if (status < 0) return 1;

                active = false;
                sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Released!\n"));
            }
        }
        if (!active) continue; // Skip drawing if not active.

        // Do drawing.
        uint32_t colour = (red << 16) | (green << 8) | blue;
        int32_t numPixels = (int32_t)(graphics.frameBufferSize >> 2);
        for (int32_t i = 0; i < numPixels; ++i) graphics.frameBuffer[i] = colour;
        drmKms_markFbDirty(&graphics);

        ++frameCounter;
        struct timespec now = {0};
        debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &now), RES == 0);
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

#include "hc/hc.h"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/drmKms.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone3_exit.c"

#include "graphics.c"

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    // Set up epoll.
    int32_t epollFd = sys_epoll_create1(0);
    if (epollFd < 0) return 1;

    uint64_t ttyNumber;
    int32_t ttyFd = -1;
    bool active = true;
    if (argc == 2) {
        // Parse TTY_NUM argument.
        if (util_strToUint(argv[1], 100, &ttyNumber) <= 0 || ttyNumber < 1 || ttyNumber > 63) {
            sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Invalid tty argument\n"));
            return 1;
        }
        char ttyPath[10] = "/dev/tty\0\0";
        ttyPath[sizeof(ttyPath) - 2] = argv[1][0];
        if (ttyNumber > 9) ttyPath[sizeof(ttyPath) - 1] = argv[1][1];

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

    struct graphics graphics;
    int64_t frameCounter = 0;
    struct timespec prev = {0};

    if (active) {
        // Initialise drawing state.
        if (graphics_init(&graphics) < 0) return 1;
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

                // Initialise graphics.
                if (graphics_init(&graphics) < 0) return 1;
                frameCounter = 0;
                debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &prev), RES == 0);
            } else if (info.ssi_signo == SIGUSR2) {
                if (!active) return 1;

                graphics_deinit(&graphics);
                status = sys_ioctl(ttyFd, VT_RELDISP, (void *)1);
                if (status < 0) return 1;

                active = false;
                sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Released!\n"));
            }
        }
        if (!active) continue; // Skip drawing if not active.

        graphics_draw(&graphics);
        ++frameCounter;
        struct timespec now = {0};
        debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &now), RES == 0);
        if (now.tv_sec > prev.tv_sec) {
            debug_printNum("FPS: ", frameCounter, "\n");
            frameCounter = 0;
            prev = now;
        }
    }
}

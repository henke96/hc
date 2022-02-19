#include "../../hc/hc.h"
#include "../../hc/libc.h"
#include "../../hc/libc/libc.c"
#include "../../hc/wrappers.c"


int32_t main(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    if (argc < 2) {
        static const char usageString[] = "Usage: ./program TTY_NUM\n";
        hc_write(STDOUT_FILENO, &usageString, sizeof(usageString) - 1);
        return 0;
    }

    // Parse TTY_NUM argument.
    int32_t ttyNumber = 0;
    for (char *s = argv[1]; *s != '\0'; ++s) {
        if (*s < '0' || *s > '9') {
            ttyNumber = 0;
            break;
        }
        ttyNumber = 10 * ttyNumber + (*s - '0');
        if (ttyNumber > 63) break;
    }
    if (ttyNumber < 1 || ttyNumber > 63) {
        static const char error[] = "Invalid TTY_NUM argument\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return 1;
    }

    // Create path and open tty.
    char ttyPath[] = "/dev/tty\0";
    ttyPath[sizeof(ttyPath) - 2] = argv[1][0];
    if (ttyNumber > 9) {
       ttyPath[sizeof(ttyPath) - 1] = argv[1][1];
    }
    int32_t ttyFd = hc_openat(AT_FDCWD, &ttyPath, O_RDWR, 0);
    if (ttyFd < 0) {
        static const char error[] = "Failed to open tty\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return -ttyFd;
    }

    // Create new session.
    int32_t status = hc_setsid();
    if (status < 0) {
        static const char error[] = "Failed to create new session (already session leader?)\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return -status;
    }

    // Set the tty as our controlling terminal.
    status = hc_ioctl(ttyFd, TIOCSCTTY, 0);
    if (status < 0) {
        static const char error[] = "Failed to set controlling terminal\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return -status;
    }

    // Set up signalfd for SIGUSR1 and SIGUSR2.
    uint64_t ttySignals = hc_SIGMASK(SIGUSR1) | hc_SIGMASK(SIGUSR2);
    status = hc_rt_sigprocmask(SIG_BLOCK, &ttySignals, NULL);
    if (status < 0) return 1;

    int32_t signalFd = hc_signalfd4(-1, &ttySignals, 0);
    if (signalFd < 0) {
        static const char error[] = "Failed to create signal fd\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return -signalFd;
    }

    // Request SIGUSR1 and SIGUSR2 when our tty is entered and left.
    struct vt_mode vtMode = {
        .mode = VT_PROCESS,
        .acqsig = SIGUSR1,
        .relsig = SIGUSR2
    };
    status = hc_ioctl(ttyFd, VT_SETMODE, &vtMode);
    if (status < 0) {
        static const char error[] = "Failed to set controlling terminal mode\n";
        hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
        return -status;
    }

    bool active = false;
    for (;;) {
        struct signalfd_siginfo info;
        int64_t numRead = hc_read(signalFd, &info, sizeof(info));
        if (numRead != sizeof(info)) {
            static const char error[] = "Failed to read from signal fd\n";
            hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
            return 1;
        }

        if (info.ssi_signo == SIGUSR1) {
            if (active) return 2;
            active = true;
            static const char message[] = "Acquired!\n";
            hc_write(STDOUT_FILENO, &message, sizeof(message) - 1);
        } else if (info.ssi_signo == SIGUSR2) {
            if (!active) return 2;

            status = hc_ioctl(ttyFd, VT_RELDISP, (void *)1);
            if (status < 0) {
                static const char error[] = "Failed to release display\n";
                hc_write(STDOUT_FILENO, &error, sizeof(error) - 1);
                return -status;
            }

            active = false;
            static const char message[] = "Released!\n";
            hc_write(STDOUT_FILENO, &message, sizeof(message) - 1);
        } else return 2;
    }

    return 0;
}
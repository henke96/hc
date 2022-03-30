#include "../../hc/hc.h"
#include "../../hc/libc.h"
#include "../../hc/libc/_start.c"
#include "../../hc/libc/small.c"
#include "../../hc/syscalls.c"
#include "../../hc/helpers/hc_sigaction.c"
#include "../../hc/libhc/util.c"
#include "../../hc/libhc/debug.c"

static void sighandler(int32_t sig) {
    debug_printNum("sighandler: ", sig, "\n");
}

static void sigaction(int32_t sig, struct siginfo *info, hc_UNUSED void *ucontext) {
    debug_printNum("sigaction: ", sig, "\n");
    debug_printNum("  si_signo: ", info->si_signo, "\n");
    debug_printNum("  si_code: ", info->si_code, "\n");
    debug_printNum("  si_errno: ", info->si_errno, "\n");
    debug_printNum("  si_pid: ", info->si_pid, "\n");
    debug_printNum("  si_uid: ", info->si_uid, "\n");
}

int32_t main(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    // Set up a `sa_handler` handler for SIGINT.
    // Test with Ctrl-C.
    {
        struct sigaction action = { .sa_handler = sighandler };
        if (hc_sigaction(SIGINT, &action, NULL) < 0) return 1;
    }

    // Set up a `sa_sigaction` handler for SIGQUIT.
    // Test with something like `kill -SIGQUIT $(pidof release.bin)`.
    {
        struct sigaction action = {
            .sa_sigaction = sigaction,
            .sa_flags = SA_SIGINFO
        };
        if (hc_sigaction(SIGQUIT, &action, NULL) < 0) return 1;
    }

    // Sleep for 10 seconds.
    struct timespec remaining = { .tv_sec = 10 };
    for (;;) {
        int32_t status = hc_clock_nanosleep(CLOCK_MONOTONIC, 0, &remaining, &remaining);
        if (status == 0) break;
        debug_ASSERT(status == -EINTR);
    }

    return 0;
}

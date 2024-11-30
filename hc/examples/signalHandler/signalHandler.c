#include "hc/hc.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_sigaction.c"
static noreturn void abort(void) {
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
}
#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"
#include "hc/debug.c"

static void sighandler(int32_t sig) {
    char print[hc_STR_LEN("sighandler: ") + util_INT32_MAX_CHARS + hc_STR_LEN("\n")];
    char *pos = hc_ARRAY_END(print);
    hc_PREPEND_STR(pos, "\n");
    pos = util_intToStr(pos, sig);
    hc_PREPEND_STR(pos, "sighandler: ");
    debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
}

static void sigaction(int32_t sig, struct siginfo *info, hc_UNUSED void *ucontext) {
    char print[
        hc_STR_LEN("sigaction: ") + util_INT32_MAX_CHARS +
        hc_STR_LEN("\n  si_signo: ") + util_INT32_MAX_CHARS +
        hc_STR_LEN("\n  si_code: ") + util_INT32_MAX_CHARS +
        hc_STR_LEN("\n  si_errno: ") + util_INT32_MAX_CHARS +
        hc_STR_LEN("\n  si_pid: ") + util_INT32_MAX_CHARS +
        hc_STR_LEN("\n  si_uid: ") + util_INT32_MAX_CHARS +
        hc_STR_LEN("\n")
    ];
    char *pos = hc_ARRAY_END(print);
    hc_PREPEND_STR(pos, "\n");
    pos = util_intToStr(pos, info->si_uid);
    hc_PREPEND_STR(pos, "\n  si_uid: ");
    pos = util_intToStr(pos, info->si_pid);
    hc_PREPEND_STR(pos, "\n  si_pid: ");
    pos = util_intToStr(pos, info->si_errno);
    hc_PREPEND_STR(pos, "\n  si_errno: ");
    pos = util_intToStr(pos, info->si_code);
    hc_PREPEND_STR(pos, "\n  si_code: ");
    pos = util_intToStr(pos, info->si_signo);
    hc_PREPEND_STR(pos, "\n  si_signo: ");
    pos = util_intToStr(pos, sig);
    hc_PREPEND_STR(pos, "sigaction: ");
    debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
}

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    // Set up a `sa_handler` handler for SIGINT.
    // Test with Ctrl-C.
    {
        struct sigaction action = { .sa_handler = sighandler };
        if (sys_sigaction(SIGINT, &action, NULL) < 0) return 1;
    }

    // Set up a `sa_sigaction` handler for SIGQUIT.
    // Test with something like `kill -SIGQUIT $(pidof signalHandler)`.
    {
        struct sigaction action = {
            .sa_sigaction = sigaction,
            .sa_flags = SA_SIGINFO
        };
        if (sys_sigaction(SIGQUIT, &action, NULL) < 0) return 1;
    }

    // Sleep for 10 seconds.
    struct timespec remaining = { .tv_sec = 10 };
    for (;;) {
        int32_t status = sys_clock_nanosleep(CLOCK_MONOTONIC, 0, &remaining, &remaining);
        if (status == 0) break;
        debug_ASSERT(status == -EINTR);
    }

    return 0;
}

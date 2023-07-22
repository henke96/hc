#include "hc/hc.h"
#include "hc/elf.h"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/vdso.c"
#include "hc/linux/helpers/_start.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    // Find the clock_gettime() function in the shared object "vDSO" provided to us by Linux.
    uint64_t *auxv = util_getAuxv(envp);
    int32_t (*clock_gettime)(int32_t clock, struct timespec *time) = vdso_lookup(auxv, vdso_CLOCK_GETTIME);
    if (clock_gettime == NULL) return 1;

    // See how many times we can call clock_gettime() in one second.
    uint64_t count = 0;
    struct timespec start = {0};
    debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &start), RES == 0);
    for (;;) {
        struct timespec current;
        debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &current), RES == 0);
        ++count;
        if (current.tv_sec > start.tv_sec && current.tv_nsec >= start.tv_nsec) break;
    }

    // Do the same test but using the syscall.
    uint64_t countSyscall = 0;
    debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &start), RES == 0);
    for (;;) {
        struct timespec current = {0};
        debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &current), RES == 0);
        ++countSyscall;
        if (current.tv_sec > start.tv_sec && current.tv_nsec >= start.tv_nsec) break;
    }

    // Print results.
    char result[34] =        "With vDSO:                       \n";
    char resultSyscall[34] = "With syscall:                    \n";
    util_uintToStr(&result[sizeof(result) - 1], count);
    util_uintToStr(&resultSyscall[sizeof(resultSyscall) - 1], countSyscall);
    struct iovec_const print[] = {
        { .iov_base = &result[0], .iov_len = sizeof(result) },
        { .iov_base = &resultSyscall[0], .iov_len = sizeof(resultSyscall) }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
    return 0;
}

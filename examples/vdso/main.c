#include "../../hc/hc.h"
#include "../../hc/libc.h"
#include "../../hc/libc/_start.c"
#include "../../hc/libc/small.c"
#include "../../hc/syscalls.c"
#include "../../hc/libhc/util.c"
#include "../../hc/libhc/elf.c"
#include "../../hc/libhc/vdso.c"

int32_t main(int32_t argc, char **argv) {
    // Find the clock_gettime() function in the shared object "vDSO" provided to us by Linux.
    uint64_t *auxv = util_getAuxv(util_getEnvp(argc, argv));
    int32_t (*clock_gettime)(int32_t clock, struct timespec *time) = vdso_lookup(auxv, vdso_CLOCK_GETTIME);
    if (clock_gettime == NULL) return 1;

    // See how many times we can call clock_gettime() in one second.
    uint64_t count = 0;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (;;) {
        struct timespec current;
        clock_gettime(CLOCK_MONOTONIC, &current);
        ++count;
        if (current.tv_sec > start.tv_sec && current.tv_nsec >= start.tv_nsec) break;
    }

    // Do the same test but using the syscall.
    uint64_t countSyscall = 0;
    hc_clock_gettime(CLOCK_MONOTONIC, &start);
    for (;;) {
        struct timespec current;
        hc_clock_gettime(CLOCK_MONOTONIC, &current);
        ++countSyscall;
        if (current.tv_sec > start.tv_sec && current.tv_nsec >= start.tv_nsec) break;
    }

    // Print results.
    char result[34] =        "With vDSO:                       \n";
    char resultSyscall[34] = "With syscall:                    \n";
    util_uintToStr(&result[sizeof(result) - 1], count);
    util_uintToStr(&resultSyscall[sizeof(resultSyscall) - 1], countSyscall);
    hc_writev(STDOUT_FILENO, (struct iovec[2]) {
        { .iov_base = (char *)&result[0], .iov_len = sizeof(result) },
        { .iov_base = (char *)&resultSyscall[0], .iov_len = sizeof(resultSyscall) }
    }, 2);
    return 0;
}

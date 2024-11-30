#include "hc/hc.h"
#include "hc/elf.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
static noreturn void abort(void) {
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
}
#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"
#include "hc/debug.c"
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
    char print[
        hc_STR_LEN("With vDSO:     ") + util_UINT64_MAX_CHARS +
        hc_STR_LEN("\nWith syscall: ") + util_UINT64_MAX_CHARS +
        hc_STR_LEN("\n")
    ];
    char *pos = hc_ARRAY_END(print);
    hc_PREPEND_STR(pos, "\n");
    pos = util_uintToStr(pos, count);
    hc_PREPEND_STR(pos, "\nWith syscall: ");
    pos = util_uintToStr(pos, countSyscall);
    hc_PREPEND_STR(pos, "With vDSO:     ");
    debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
    return 0;
}

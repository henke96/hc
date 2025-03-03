#include "hc/hc.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
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
#include "hc/linux/helpers/_start.c"

static int64_t tests_currentNs(void) {
    struct timespec timespec = {0};
    debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &timespec), RES == 0);
    return timespec.tv_sec * 1000000000 + timespec.tv_nsec;
}

#include "../common/common.c"

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    common_parseArgs(argc, argv);
    common_tests();
    return 0;
}

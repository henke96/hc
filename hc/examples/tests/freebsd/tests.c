#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#include "hc/freebsd/_start.c"
#include "hc/freebsd/debug.c"

static int64_t tests_currentNs(void) {
    struct timespec timespec = {0};
    debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &timespec), RES == 0);
    return timespec.tv_sec * 1000000000 + timespec.tv_nsec;
}

#include "../common/common.c"

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    common_parseArgs(argc, argv);
    common_tests();
    return 0;
}

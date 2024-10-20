#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mul128.c"
#include "hc/wasm/wasi/wasi.h"
#include "hc/wasm/wasi/debug.c"

static int64_t tests_currentNs(void) {
    int64_t timestamp;
    debug_CHECK(clock_time_get(CLOCK_MONOTONIC, 1, &timestamp), RES == 0);
    return timestamp;
}

#include "../common/common.c"

void noreturn _start(void) {
    char *argv[] = { "tests", "1", NULL }; // TODO
    common_parseArgs(hc_ARRAY_LEN(argv) - 1, &argv[0]);
    common_tests();
    proc_exit(0);
}

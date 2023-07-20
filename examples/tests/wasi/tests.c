#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compiler_rt/libc.c"
#include "hc/compiler_rt/mul128.c"
#include "hc/wasm/wasi/wasi.h"
#include "hc/wasm/wasi/debug.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/sha1.c"
#include "hc/crypto/curve25519.c"
#include "hc/crypto/x25519.c"
#include "hc/crypto/ed25519.c"
#include "hc/crypto/chacha20.c"
#include "hc/crypto/poly1305.c"
#include "hc/crypto/aes.c"
#include "hc/crypto/aes128.c"
#include "hc/crypto/aes256.c"
#include "hc/base64.c"

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

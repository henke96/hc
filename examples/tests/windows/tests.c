#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compiler_rt/libc.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"
#include "hc/windows/_start.c"

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

static int64_t tests_timestampMult;
static int64_t tests_currentNs(void) {
    int64_t time;
    debug_CHECK(QueryPerformanceCounter(&time), RES != 0);
    return time * tests_timestampMult;
}

#include "../common/common.c"

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    int64_t timerFrequency = 0;
    if (QueryPerformanceFrequency(&timerFrequency) == 0) debug_abort();
    if (timerFrequency == 0 || 1000000000 % timerFrequency != 0) debug_abort();
    tests_timestampMult = 1000000000 / timerFrequency;

    common_parseArgs(argc, argv);
    common_tests();
    return 0;
}

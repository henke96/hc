#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"

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

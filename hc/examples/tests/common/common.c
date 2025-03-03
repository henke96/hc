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

#define CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)

#define TIME(FN) { \
    int64_t startTime = tests_currentNs(); \
    FN(); \
    int64_t duration = tests_currentNs() - startTime; \
    char *unit; \
    if (duration >= 100000000) { \
        unit = " ms\n"; \
        duration = (duration + 500000) / 1000000; \
    } else if (duration >= 100000) { \
        unit = " us\n"; \
        duration = (duration + 500) / 1000; \
    } else unit = " ns\n"; \
    char print[ \
        hc_STR_LEN(#FN " took ") + \
        util_INT64_MAX_CHARS + \
        hc_STR_LEN(" xx\n") \
    ]; \
    char *pos = hc_ARRAY_END(print); \
    hc_PREPEND(pos, unit, hc_STR_LEN(" xx\n")); \
    pos = util_intToStr(pos, duration); \
    hc_PREPEND_STR(pos, #FN " took "); \
    if (util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos) < 0) util_abort(); \
}

static uint64_t tests_level = 0;

#include "memset.c"
#include "memmove.c"
#include "base64.c"
#include "crypto/sha.c"
#include "crypto/x25519.c"
#include "crypto/ed25519.c"
#include "crypto/chacha20.c"
#include "crypto/poly1305.c"
#include "crypto/aes.c"

static void common_parseArgs(int32_t argc, char **argv) {
    if (argc >= 2) {
        if (util_strToUint(argv[1], util_UINT64_MAX_CHARS, &tests_level) < 0) {
            tests_level = UINT64_MAX; // Overflow.
        }
    }
}

static void common_tests(void) {
    TIME(memset_tests)
    TIME(memmove_tests)
    TIME(base64_tests)
    TIME(sha_tests)
    TIME(x25519_tests)
    TIME(ed25519_tests)
    TIME(chacha20_tests)
    TIME(poly1305_tests)
    TIME(aes_tests)

    if (util_writeAll(util_STDERR, hc_STR_COMMA_LEN("All tests passed!\n")) < 0) util_abort();
}

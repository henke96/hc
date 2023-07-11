#define CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)

#define TIME(FN) { \
    int64_t startTime = tests_currentNs(); \
    FN(); \
    int64_t duration = tests_currentNs() - startTime; \
    debug_print(#FN); \
    if (duration >= 100000000) debug_printNum(" took ", (duration + 500000) / 1000000, " ms\n"); \
    else if (duration >= 100000) debug_printNum(" took ", (duration + 500) / 1000, " us\n"); \
    else debug_printNum(" took ", duration, " ns\n"); \
}

static uint64_t tests_level = 0;

#include "crypto/sha.c"
#include "crypto/x25519.c"
#include "crypto/ed25519.c"
#include "crypto/chacha20.c"
#include "crypto/poly1305.c"

static void common_parseArgs(int32_t argc, char **argv) {
    if (argc >= 2) {
        if (util_strToUint(argv[1], util_UINT64_MAX_CHARS, &tests_level) < 0) {
            tests_level = UINT64_MAX; // Overflow.
        }
    }
}

static void common_tests(void) {
    TIME(sha_tests)
    TIME(x25519_tests)
    TIME(ed25519_tests)
    TIME(chacha20_tests)
    TIME(poly1305_tests)

    debug_print("All tests passed!\n");
}

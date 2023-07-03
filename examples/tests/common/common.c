#define CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)

#include "crypto/sha.c"
#include "crypto/x25519.c"
#include "crypto/ed25519.c"
#include "crypto/chacha20.c"
#include "crypto/poly1305.c"

static void common_tests(int32_t argc, char **argv, uint64_t *level) {
    int32_t parsed = 0;
    if (argc >= 2) {
        parsed = util_strToUint(argv[1], util_UINT64_MAX_CHARS, level);
        if (parsed < 0) *level = UINT64_MAX; // Overflow.
    }
    if (parsed == 0) *level = 0;

    sha_tests(*level);
    x25519_tests(*level);
    ed25519_tests(*level);
    chacha20_tests(*level);
    poly1305_tests(*level);
}

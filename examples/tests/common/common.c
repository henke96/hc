#define CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)

#include "crypto/sha.c"
#include "crypto/curve25519.c"

static void common_tests(void) {
    sha_tests();
    curve25519_tests();
}

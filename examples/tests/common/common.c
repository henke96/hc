#define CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)

#include "crypto/sha.c"

int32_t common_tests(void) {
    sha_tests();
    return 0;
}

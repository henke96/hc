#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/mem.c"
#include "hc/libc/small.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/curve25519.c"
#include "hc/crypto/x25519.c"
#include "hc/crypto/ed25519.c"
#include "hc/crypto/chacha20.c"
#include "hc/crypto/poly1305.c"

#include "../common/common.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    uint64_t level;
    common_tests(argc, argv, &level);
    sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Tests passed!\n"));
    return 0;
}

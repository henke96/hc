#include "hc/hc.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/sha1.c"

static char buffer[65536] hc_ALIGNED(16);

union {
    struct sha512 sha512;
    struct sha256 sha256;
    struct sha1 sha1;
} state;

enum hashFunction {
    SHA1,
    SHA256,
    SHA512
};

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    if (argc < 3) return 1;

    enum hashFunction hashFunction;
    int32_t hashSize;
    if (util_cstrCmp(argv[1], "1") == 0) {
        hashFunction = SHA1;
        hashSize = sha1_HASH_SIZE;
        sha1_init(&state.sha1);
    } else if (util_cstrCmp(argv[1], "256") == 0) {
        hashFunction = SHA256;
        hashSize = sha256_HASH_SIZE;
        sha256_init(&state.sha256);
    } else if (util_cstrCmp(argv[1], "384") == 0) {
        hashFunction = SHA512;
        hashSize = 48;
        sha512_init384(&state.sha512);
    } else if (util_cstrCmp(argv[1], "512") == 0) {
        hashFunction = SHA512;
        hashSize = sha512_HASH_SIZE;
        sha512_init(&state.sha512);
    } else return 2;

    int32_t fd = sys_openat(AT_FDCWD, argv[2], O_RDONLY, 0);
    if (fd < 0) return 3;

    for (;;) {
        int64_t numRead = sys_read(fd, &buffer[0], sizeof(buffer));
        if (numRead <= 0) {
            if (numRead < 0) return 4;
            break;
        }

        switch (hashFunction) {
            case SHA1: sha1_update(&state.sha1, &buffer[0], numRead); break;
            case SHA256: sha256_update(&state.sha256, &buffer[0], numRead); break;
            case SHA512: sha512_update(&state.sha512, &buffer[0], numRead); break;
            default: hc_UNREACHABLE;
        }
    }

    uint8_t hash[sha512_HASH_SIZE];
    switch (hashFunction) {
        case SHA1: sha1_finish(&state.sha1, &hash[0]); break;
        case SHA256: sha256_finish(&state.sha256, &hash[0]); break;
        case SHA512: sha512_finish(&state.sha512, &hash[0]); break;
        default: hc_UNREACHABLE;
    }


    for (int32_t i = 0; i < hashSize; ++i) {
        int32_t bufferI = 2 * i;
        buffer[bufferI] = util_hexTable[hash[i] >> 4];
        buffer[bufferI + 1] = util_hexTable[hash[i] & 0xF];
    }
    int32_t hexSize = 2 * hashSize;
    buffer[hexSize] = '\n';
    if (sys_write(STDOUT_FILENO, &buffer[0], hexSize + 1) != hexSize + 1) return 5;
    return 0;
}

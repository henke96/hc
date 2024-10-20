static int32_t init(char *file);
static void deinit(void);
static int32_t readIntoBuffer(void);
static int32_t printBuffer(int32_t size);

static char buffer[65536] hc_ALIGNED(16);

union {
    struct sha512 sha512;
    struct sha256 sha256;
    struct sha1 sha1;
} hashState;

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
        sha1_init(&hashState.sha1);
    } else if (util_cstrCmp(argv[1], "256") == 0) {
        hashFunction = SHA256;
        hashSize = sha256_HASH_SIZE;
        sha256_init(&hashState.sha256);
    } else if (util_cstrCmp(argv[1], "384") == 0) {
        hashFunction = SHA512;
        hashSize = 48;
        sha512_init384(&hashState.sha512);
    } else if (util_cstrCmp(argv[1], "512") == 0) {
        hashFunction = SHA512;
        hashSize = sha512_HASH_SIZE;
        sha512_init(&hashState.sha512);
    } else return 2;

    if (init(argv[2]) < 0) return 3;

    int32_t status = 4;
    for (;;) {
        int32_t numRead = readIntoBuffer();
        if (numRead <= 0) {
            if (numRead < 0) goto cleanup;
            break;
        }

        switch (hashFunction) {
            case SHA1: sha1_update(&hashState.sha1, &buffer[0], numRead); break;
            case SHA256: sha256_update(&hashState.sha256, &buffer[0], numRead); break;
            case SHA512: sha512_update(&hashState.sha512, &buffer[0], numRead); break;
            default: hc_UNREACHABLE;
        }
    }

    uint8_t hash[sha512_HASH_SIZE];
    switch (hashFunction) {
        case SHA1: sha1_finish(&hashState.sha1, &hash[0]); break;
        case SHA256: sha256_finish(&hashState.sha256, &hash[0]); break;
        case SHA512: sha512_finish(&hashState.sha512, &hash[0]); break;
        default: hc_UNREACHABLE;
    }

    for (int32_t i = 0; i < hashSize; ++i) {
        int32_t bufferI = 2 * i;
        buffer[bufferI] = util_hexTable[hash[i] >> 4];
        buffer[bufferI + 1] = util_hexTable[hash[i] & 0xF];
    }
    int32_t hexSize = 2 * hashSize;
    buffer[hexSize] = '\n';
    if (printBuffer(hexSize + 1) < 0) goto cleanup;

    status = 0;
    cleanup:
    deinit();
    return status;
}

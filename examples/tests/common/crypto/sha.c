static void _sha_test(
    const uint8_t *input,
    int64_t inputSize,
    int64_t repetitions,
    const uint8_t *sha1Expected,
    const uint8_t *sha256Expected,
    const uint8_t *sha512Expected
) {
    uint8_t sha1Hash[sha1_HASH_SIZE];
    struct sha1 sha1;
    sha1_init(&sha1);
    for (int64_t i = 0; i < repetitions; ++i) sha1_update(&sha1, &input[0], inputSize);
    sha1_finish(&sha1, &sha1Hash[0]);
    CHECK(hc_MEMCMP(&sha1Hash[0], &sha1Expected[0], sha1_HASH_SIZE), RES == 0);

    uint8_t sha256Hash[sha256_HASH_SIZE];
    struct sha256 sha256;
    sha256_init(&sha256);
    for (int64_t i = 0; i < repetitions; ++i) sha256_update(&sha256, &input[0], inputSize);
    sha256_finish(&sha256, &sha256Hash[0]);
    CHECK(hc_MEMCMP(&sha256Hash[0], &sha256Expected[0], sha256_HASH_SIZE), RES == 0);

    uint8_t sha512Hash[sha512_HASH_SIZE];
    struct sha512 sha512;
    sha512_init(&sha512);
    for (int64_t i = 0; i < repetitions; ++i) sha512_update(&sha512, &input[0], inputSize);
    sha512_finish(&sha512, &sha512Hash[0]);
    CHECK(hc_MEMCMP(&sha512Hash[0], &sha512Expected[0], sha512_HASH_SIZE), RES == 0);
}

static void sha_tests(void) {
    _sha_test(
        (const uint8_t *)hc_STR_COMMA_LEN(""),
        1,
        (const uint8_t *)"\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09",
        (const uint8_t *)"\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55",
        (const uint8_t *)"\xcf\x83\xe1\x35\x7e\xef\xb8\xbd\xf1\x54\x28\x50\xd6\x6d\x80\x07\xd6\x20\xe4\x05\x0b\x57\x15\xdc\x83\xf4\xa9\x21\xd3\x6c\xe9\xce\x47\xd0\xd1\x3c\x5d\x85\xf2\xb0\xff\x83\x18\xd2\x87\x7e\xec\x2f\x63\xb9\x31\xbd\x47\x41\x7a\x81\xa5\x38\x32\x7a\xf9\x27\xda\x3e"
    );
    _sha_test(
        (const uint8_t *)hc_STR_COMMA_LEN("abc"),
        1,
        (const uint8_t *)"\xa9\x99\x3e\x36\x47\x06\x81\x6a\xba\x3e\x25\x71\x78\x50\xc2\x6c\x9c\xd0\xd8\x9d",
        (const uint8_t *)"\xba\x78\x16\xbf\x8f\x01\xcf\xea\x41\x41\x40\xde\x5d\xae\x22\x23\xb0\x03\x61\xa3\x96\x17\x7a\x9c\xb4\x10\xff\x61\xf2\x00\x15\xad",
        (const uint8_t *)"\xdd\xaf\x35\xa1\x93\x61\x7a\xba\xcc\x41\x73\x49\xae\x20\x41\x31\x12\xe6\xfa\x4e\x89\xa9\x7e\xa2\x0a\x9e\xee\xe6\x4b\x55\xd3\x9a\x21\x92\x99\x2a\x27\x4f\xc1\xa8\x36\xba\x3c\x23\xa3\xfe\xeb\xbd\x45\x4d\x44\x23\x64\x3c\xe8\x0e\x2a\x9a\xc9\x4f\xa5\x4c\xa4\x9f"
    );
    _sha_test(
        (const uint8_t *)hc_STR_COMMA_LEN("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        1,
        (const uint8_t *)"\x84\x98\x3e\x44\x1c\x3b\xd2\x6e\xba\xae\x4a\xa1\xf9\x51\x29\xe5\xe5\x46\x70\xf1",
        (const uint8_t *)"\x24\x8d\x6a\x61\xd2\x06\x38\xb8\xe5\xc0\x26\x93\x0c\x3e\x60\x39\xa3\x3c\xe4\x59\x64\xff\x21\x67\xf6\xec\xed\xd4\x19\xdb\x06\xc1",
        (const uint8_t *)"\x20\x4a\x8f\xc6\xdd\xa8\x2f\x0a\x0c\xed\x7b\xeb\x8e\x08\xa4\x16\x57\xc1\x6e\xf4\x68\xb2\x28\xa8\x27\x9b\xe3\x31\xa7\x03\xc3\x35\x96\xfd\x15\xc1\x3b\x1b\x07\xf9\xaa\x1d\x3b\xea\x57\x78\x9c\xa0\x31\xad\x85\xc7\xa7\x1d\xd7\x03\x54\xec\x63\x12\x38\xca\x34\x45"
    );
    _sha_test(
        (const uint8_t *)hc_STR_COMMA_LEN("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"),
        1,
        (const uint8_t *)"\xa4\x9b\x24\x46\xa0\x2c\x64\x5b\xf4\x19\xf9\x95\xb6\x70\x91\x25\x3a\x04\xa2\x59",
        (const uint8_t *)"\xcf\x5b\x16\xa7\x78\xaf\x83\x80\x03\x6c\xe5\x9e\x7b\x04\x92\x37\x0b\x24\x9b\x11\xe8\xf0\x7a\x51\xaf\xac\x45\x03\x7a\xfe\xe9\xd1",
        (const uint8_t *)"\x8e\x95\x9b\x75\xda\xe3\x13\xda\x8c\xf4\xf7\x28\x14\xfc\x14\x3f\x8f\x77\x79\xc6\xeb\x9f\x7f\xa1\x72\x99\xae\xad\xb6\x88\x90\x18\x50\x1d\x28\x9e\x49\x00\xf7\xe4\x33\x1b\x99\xde\xc4\xb5\x43\x3a\xc7\xd3\x29\xee\xb6\xdd\x26\x54\x5e\x96\xe5\x5b\x87\x4b\xe9\x09"
    );
    if (tests_level >= 1) _sha_test(
        (const uint8_t *)hc_STR_COMMA_LEN("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        5000,
        (const uint8_t *)"\x34\xaa\x97\x3c\xd4\xc4\xda\xa4\xf6\x1e\xeb\x2b\xdb\xad\x27\x31\x65\x34\x01\x6f",
        (const uint8_t *)"\xcd\xc7\x6e\x5c\x99\x14\xfb\x92\x81\xa1\xc7\xe2\x84\xd7\x3e\x67\xf1\x80\x9a\x48\xa4\x97\x20\x0e\x04\x6d\x39\xcc\xc7\x11\x2c\xd0",
        (const uint8_t *)"\xe7\x18\x48\x3d\x0c\xe7\x69\x64\x4e\x2e\x42\xc7\xbc\x15\xb4\x63\x8e\x1f\x98\xb1\x3b\x20\x44\x28\x56\x32\xa8\x03\xaf\xa9\x73\xeb\xde\x0f\xf2\x44\x87\x7e\xa6\x0a\x4c\xb0\x43\x2c\xe5\x77\xc3\x1b\xeb\x00\x9c\x5c\x2c\x49\xaa\x2e\x4e\xad\xb2\x17\xad\x8c\xc0\x9b"
    );
    if (tests_level >= 2) _sha_test(
        (const uint8_t *)hc_STR_COMMA_LEN("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno"),
        16777216,
        (const uint8_t *)"\x77\x89\xf0\xc9\xef\x7b\xfc\x40\xd9\x33\x11\x14\x3d\xfb\xe6\x9e\x20\x17\xf5\x92",
        (const uint8_t *)"\x50\xe7\x2a\x0e\x26\x44\x2f\xe2\x55\x2d\xc3\x93\x8a\xc5\x86\x58\x22\x8c\x0c\xbf\xb1\xd2\xca\x87\x2a\xe4\x35\x26\x6f\xcd\x05\x5e",
        (const uint8_t *)"\xb4\x7c\x93\x34\x21\xea\x2d\xb1\x49\xad\x6e\x10\xfc\xe6\xc7\xf9\x3d\x07\x52\x38\x01\x80\xff\xd7\xf4\x62\x9a\x71\x21\x34\x83\x1d\x77\xbe\x60\x91\xb8\x19\xed\x35\x2c\x29\x67\xa2\xe2\xd4\xfa\x50\x50\x72\x3c\x96\x30\x69\x1f\x1a\x05\xa7\x28\x1d\xbe\x6c\x10\x86"
    );
}

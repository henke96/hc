static void _poly1305_test(const uint8_t *input, int64_t inputSize, const uint8_t *key, const uint8_t *expected) {
    uint8_t mac[poly1305_MAC_SIZE];
    struct poly1305 poly1305;

    poly1305_init(&poly1305, &key[0]);
    poly1305_update(&poly1305, &input[0], inputSize);
    poly1305_finish(&poly1305, &mac[0]);
    CHECK(hc_MEMCMP(&mac[0], &expected[0], poly1305_MAC_SIZE), RES == 0);

    // Again but incremental.
    poly1305_init(&poly1305, &key[0]);
    poly1305_update(&poly1305, &input[0], 1);
    poly1305_update(&poly1305, &input[1], inputSize - 1);
    poly1305_finish(&poly1305, &mac[0]);
    CHECK(hc_MEMCMP(&mac[0], &expected[0], poly1305_MAC_SIZE), RES == 0);
}

static void poly1305_tests(void) {
    // Test vector from RFC 7539.
    _poly1305_test(
        (uint8_t *)hc_STR_COMMA_LEN("Cryptographic Forum Research Group"),
        &((uint8_t[]) { 0x85, 0xd6, 0xbe, 0x78, 0x57, 0x55, 0x6d, 0x33, 0x7f, 0x44, 0x52, 0xfe, 0x42, 0xd5, 0x06, 0xa8, 0x01, 0x03, 0x80, 0x8a, 0xfb, 0x0d, 0xb2, 0xfd, 0x4a, 0xbf, 0xf6, 0xaf, 0x41, 0x49, 0xf5, 0x1b })[0],
        &((uint8_t[]) { 0xa8, 0x06, 0x1d, 0xc1, 0x30, 0x51, 0x36, 0xc6, 0xc2, 0x2b, 0x8b, 0xaf, 0x0c, 0x01, 0x27, 0xa9 })[0]
    );
}

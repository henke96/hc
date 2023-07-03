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

static void poly1305_tests(hc_UNUSED uint64_t level) {
    // Test vector from RFC 8439.
    _poly1305_test(
        (const uint8_t *)hc_STR_COMMA_LEN("Cryptographic Forum Research Group"),
        (const uint8_t *)"\x85\xd6\xbe\x78\x57\x55\x6d\x33\x7f\x44\x52\xfe\x42\xd5\x06\xa8\x01\x03\x80\x8a\xfb\x0d\xb2\xfd\x4a\xbf\xf6\xaf\x41\x49\xf5\x1b",
        (const uint8_t *)"\xa8\x06\x1d\xc1\x30\x51\x36\xc6\xc2\x2b\x8b\xaf\x0c\x01\x27\xa9"
    );
    // Test vectors from draft-agl-tls-chacha20poly1305-04.
    _poly1305_test(
        (const uint8_t *)hc_STR_COMMA_LEN("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"),
        (const uint8_t *)"\x74\x68\x69\x73\x20\x69\x73\x20\x33\x32\x2d\x62\x79\x74\x65\x20\x6b\x65\x79\x20\x66\x6f\x72\x20\x50\x6f\x6c\x79\x31\x33\x30\x35",
        (const uint8_t *)"\x49\xec\x78\x09\x0e\x48\x1e\xc6\xc2\x6b\x33\xb9\x1c\xcc\x03\x07"
    );
    _poly1305_test(
        (const uint8_t *)hc_STR_COMMA_LEN("\x48\x65\x6c\x6c\x6f\x20\x77\x6f\x72\x6c\x64\x21"),
        (const uint8_t *)"\x74\x68\x69\x73\x20\x69\x73\x20\x33\x32\x2d\x62\x79\x74\x65\x20\x6b\x65\x79\x20\x66\x6f\x72\x20\x50\x6f\x6c\x79\x31\x33\x30\x35",
        (const uint8_t *)"\xa6\xf7\x45\x00\x8f\x81\xc9\x16\xa2\x0d\xcc\x74\xee\xf2\xb2\xf0"
    );
}

static void _base64_test(const char *input, ssize_t inputSize, const char *expected) {
    char buffer[64];
    ssize_t outputSize = base64_ENCODE_SIZE(inputSize);
    CHECK(outputSize, (size_t)RES <= sizeof(buffer));

    // Copy input into buffer to test inplace encoding.
    char *inputBuffer = &buffer[outputSize - inputSize];
    hc_MEMCPY(inputBuffer, input, (size_t)inputSize);

    base64_encode(&buffer[0], inputBuffer, inputSize);
    CHECK(mem_compare(&buffer[0], expected, (size_t)outputSize), RES == 0);
}

static void base64_tests(void) {
    // Test vectors from RFC 4648.
    _base64_test(hc_STR_COMMA_LEN(""), "");
    _base64_test(hc_STR_COMMA_LEN("f"), "Zg==");
    _base64_test(hc_STR_COMMA_LEN("fo"), "Zm8=");
    _base64_test(hc_STR_COMMA_LEN("foo"), "Zm9v");
    _base64_test(hc_STR_COMMA_LEN("foob"), "Zm9vYg==");
    _base64_test(hc_STR_COMMA_LEN("fooba"), "Zm9vYmE=");
    _base64_test(hc_STR_COMMA_LEN("foobar"), "Zm9vYmFy");
}

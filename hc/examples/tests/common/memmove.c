#define _memmove_SIZE 256

static void _memmove_check(char *buffer, int32_t size, int32_t offset) {
    int32_t i = 0;
    for (; i < 32 + offset; ++i) CHECK(buffer[i], RES == (i & 0xFF));
    for (; i < 32 + offset + size; ++i) CHECK(buffer[i], RES == ((i - offset) & 0xFF));
    for (; i < 32 + _memmove_SIZE + 32; ++i) CHECK(buffer[i], RES == (i & 0xFF));
}

static void memmove_tests(void) {
    char buffer[32 + _memmove_SIZE + 32] hc_ALIGNED(16);

    for (int32_t size = 0; size <= _memmove_SIZE; ++size) {
        for (int32_t offset = -15; offset < 16; ++offset) {
            for (uint32_t i = 0; i < sizeof(buffer); ++i) buffer[i] = i & 0xFF;

            hc_MEMMOVE(&buffer[32 + offset], &buffer[32], (size_t)size);
            _memmove_check(&buffer[0], size, offset);
        }
    }
}

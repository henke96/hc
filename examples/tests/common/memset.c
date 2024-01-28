#define _memset_ITERATIONS 1
#define _memset_SIZE 256

static void _memset_check(char *buffer, int32_t size, int32_t offset, int32_t val) {
    char paddingVal = ~(char)val;
    int32_t i = 0;
    for (; i < 16 + offset; ++i) CHECK(buffer[i], RES == paddingVal);
    for (; i < 16 + offset + size; ++i) CHECK(buffer[i], RES == val);
    for (; i < 16 + _memset_SIZE + 32; ++i) CHECK(buffer[i], RES == paddingVal);
}

static void memset_tests(void) {
    char buffer[16 + _memset_SIZE + 32] hc_ALIGNED(16);

    for (int32_t size = 0; size <= _memset_SIZE; ++size) {
        for (int32_t offset = 0; offset < 16; ++offset) {
            int32_t val = 0xAA;
            hc_MEMSET(&buffer[0], ~val, sizeof(buffer));
            for (int32_t i = 0; i < _memset_ITERATIONS; ++i) {
                hc_MEMSET(&buffer[16 + offset], val, (size_t)size);
            }
            _memset_check(&buffer[0], size, offset, val);
        }
    }
}

#define test_RESERVE_SIZE 65536

static int32_t test(void) {
    struct allocator alloc;
    if (allocator_init(&alloc, test_RESERVE_SIZE) < 0) return -1;

    char *mem = alloc.mem;

    if (allocator_resize(&alloc, 16000) < 0) return -2;
    mem[16000 - 1] = 'a';
    if (allocator_resize(&alloc, 8000) < 0) return -3;
    mem[8000 - 1] = 'a';
    if (allocator_resize(&alloc, 65536) < 0) return -4;
    mem[65536 - 1] = 'a';

    allocator_deinit(&alloc, test_RESERVE_SIZE);
    return 0;
}

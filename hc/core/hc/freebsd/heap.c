static void *heap_reserve(int64_t size) {
    void *mem = mmap(NULL, size, PROT_NONE, MAP_GUARD, -1, 0);
    if ((int64_t)mem < 0) return NULL;
    return mem;
}

static void heap_unreserve(void *address, int64_t size) {
    debug_CHECK(munmap(address, size), RES == 0);
}

static int32_t heap_commit(void *address, int64_t size) {
    void *mem = mmap(address, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ((int64_t)mem < 0) return -1;
    return 0;
}

static void heap_decommit(void *address, int64_t size) {
    debug_CHECK(
        mmap(address, size, PROT_NONE, MAP_FIXED | MAP_GUARD, -1, 0),
        RES == address
    );
}

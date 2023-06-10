static void *heap_reserve(int64_t size) {
    void *mem = sys_mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)mem < 0) return NULL;
    return mem;
}

static void heap_unreserve(void *address, int64_t size) {
    debug_CHECK(sys_munmap(address, size), RES == 0);
}

static int32_t heap_commit(void *address, int64_t size) {
    return sys_mprotect(address, size, PROT_READ | PROT_WRITE);
}

static void heap_decommit(void *address, int64_t size) {
    debug_CHECK(
        sys_mmap(address, size, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, 0, 0),
        RES == address
    );
}

static void *heap_reserve(int64_t size) {
    void *mem = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ((int64_t)mem < 0) return NULL;
    return mem;
}

static void heap_unreserve(void *address, int64_t size) {
    debug_CHECK(munmap(address, size), RES == 0);
}

// TODO: Is this a good way to implement heap_commit and heap_decommit on FreeBSD?
static int32_t heap_commit(void *address, int64_t len) {
    return mprotect(address, len, PROT_READ | PROT_WRITE);
}

static void heap_decommit(void *address, int64_t len) {
    debug_CHECK(
        mmap(address, len, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        RES == address
    );
}

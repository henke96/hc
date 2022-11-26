static void *heap_reserve(int64_t size) {
    return VirtualAlloc(NULL, (uint64_t)size, MEM_RESERVE, PAGE_NOACCESS);
}

static void heap_unreserve(void *address, hc_UNUSED int64_t size) {
    debug_CHECK(VirtualFree(address, 0, MEM_RELEASE), RES != 0);
}

static int32_t heap_commit(void *address, int64_t len) {
    void *res = VirtualAlloc(address, (uint64_t)len, MEM_COMMIT, PAGE_READWRITE);
    if (res == NULL) return -1;
    return 0;
}

static void heap_decommit(void *address, int64_t len) {
    debug_CHECK(
        VirtualFree(address, (uint64_t)len, MEM_DECOMMIT),
        RES != 0
    );
}

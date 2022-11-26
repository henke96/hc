// https://github.com/WebAssembly/design/issues/1397

#define heap_WASM_PAGE_SHIFT 16
#define heap_WASM_PAGE_SIZE (1 << (heap_WASM_PAGE_SHIFT))

static void *heap_reserve(int64_t size) {
    uint32_t numPages = (uint32_t)(((uint64_t)size + heap_WASM_PAGE_SIZE - 1) >> heap_WASM_PAGE_SHIFT);
    void *mem = (void *)hc_WASM_MEMORY_GROW(numPages);
    if ((int64_t)mem < 0) return NULL;
    return mem;
}

static void heap_unreserve(hc_UNUSED void *address, hc_UNUSED int64_t size) {}

static int32_t heap_commit(hc_UNUSED void *address, hc_UNUSED int64_t len) {
    return 0;
}

static void heap_decommit(hc_UNUSED void *address, hc_UNUSED int64_t len) {}

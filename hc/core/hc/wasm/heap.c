// https://github.com/WebAssembly/design/issues/1397

#define heap_WASM_PAGE_SHIFT 16
#define heap_WASM_PAGE_SIZE (1 << (heap_WASM_PAGE_SHIFT))

static void *heap_reserve(int32_t size) {
    uint32_t numPages = ((uint32_t)size + heap_WASM_PAGE_SIZE - 1) >> heap_WASM_PAGE_SHIFT;
    int32_t mem = (int32_t)hc_WASM_MEMORY_GROW(numPages);
    if (mem == -1) return NULL;
    return (void *)mem;
}

static void heap_unreserve(hc_UNUSED void *address, hc_UNUSED int32_t size) {}

static int32_t heap_commit(hc_UNUSED void *address, hc_UNUSED int32_t size) {
    return 0;
}

static void heap_decommit(hc_UNUSED void *address, hc_UNUSED int32_t size) {}

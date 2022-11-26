// For now we always allocate in pages of 4096 bytes.
#define allocator_PAGE_SIZE 4096

struct allocator {
    void *mem;
    hc_ILP32_PAD(__pad);
    int64_t size;
    int64_t maxSize;
};

static int32_t allocator_init(struct allocator *self, int64_t maxSize) {
    self->maxSize = maxSize;
    self->size = 0;
    self->mem = heap_reserve(maxSize);
    if (self->mem == NULL) return -1;
    return 0;
}

static void allocator_deinit(struct allocator *self) {
    heap_unreserve(self->mem, self->maxSize);
}

static int32_t allocator_resize(struct allocator *self, int64_t newSize) {
    newSize = math_ALIGN_FORWARD(newSize, allocator_PAGE_SIZE);

    int64_t diff = newSize - self->size;
    if (diff == 0) return 0;

    if (diff < 0) {
        heap_decommit(self->mem + newSize, -diff);
    } else {
        if (newSize > self->maxSize) return -1;
        int32_t status = heap_commit(self->mem + self->size, diff);
        if (status < 0) return -1;
    }
    self->size = newSize;
    return 0;
}

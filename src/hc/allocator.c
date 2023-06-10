#ifndef allocator_PAGE_SIZE
    #error "Please define `allocator_PAGE_SIZE`"
#endif

struct allocator {
    void *mem;
    hc_ILP32_PAD(__pad)
    int64_t size;
};

static int32_t allocator_init(struct allocator *self, int64_t reserveSize) {
    reserveSize = math_ALIGN_FORWARD(reserveSize, (int64_t)allocator_PAGE_SIZE);
    self->size = 0;
    self->mem = heap_reserve(reserveSize);
    if (self->mem == NULL) return -1;
    return 0;
}

static void allocator_deinit(struct allocator *self, int64_t reserveSize) {
    reserveSize = math_ALIGN_FORWARD(reserveSize, (int64_t)allocator_PAGE_SIZE);
    debug_ASSERT(self->size <= reserveSize);
    heap_unreserve(self->mem, reserveSize);
}

static int32_t allocator_resize(struct allocator *self, int64_t newSize) {
    newSize = math_ALIGN_FORWARD(newSize, (int64_t)allocator_PAGE_SIZE);

    int64_t diff = newSize - self->size;
    if (diff == 0) return 0;

    if (diff < 0) {
        heap_decommit(self->mem + newSize, -diff);
    } else {
        int32_t status = heap_commit(self->mem + self->size, diff);
        if (status < 0) return -1;
    }
    self->size = newSize;
    return 0;
}

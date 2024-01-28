#ifndef allocator_PAGE_SIZE
    #error "Please define `allocator_PAGE_SIZE`"
#endif

struct allocator {
    void *mem;
    ssize_t size;
    ssize_t reserveSize;
};

static int32_t allocator_init(struct allocator *self, ssize_t reserveSize) {
    self->reserveSize = math_ALIGN_FORWARD(reserveSize, (ssize_t)allocator_PAGE_SIZE);
    self->mem = heap_reserve(self->reserveSize);
    if (self->mem == NULL) return -1;
    self->size = 0;
    return 0;
}

static void allocator_deinit(struct allocator *self) {
    heap_unreserve(self->mem, self->reserveSize);
}

static int32_t allocator_resize(struct allocator *self, ssize_t newSize) {
    ssize_t commitSize = math_ALIGN_FORWARD(self->size, (ssize_t)allocator_PAGE_SIZE);
    ssize_t newCommitSize = math_ALIGN_FORWARD(newSize, (ssize_t)allocator_PAGE_SIZE);

    ssize_t diff = newCommitSize - commitSize;
    if (diff == 0) goto out;

    if (diff < 0) {
        heap_decommit(self->mem + newCommitSize, -diff);
    } else {
        if (newCommitSize > self->reserveSize) return -1;
        int32_t status = heap_commit(self->mem + commitSize, diff);
        if (status < 0) return -1;
    }
    out:
    self->size = newSize;
    return 0;
}

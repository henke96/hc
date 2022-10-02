struct xauth {
    uint8_t *mappedFile;
    int64_t fileSize;
    int64_t fileOffset;
    int32_t fd;
    int32_t __pad;
};

struct xauth_entry {
    uint8_t *name;
    uint8_t *data;
    uint16_t nameLength;
    uint16_t dataLength;
    int32_t __pad;
};

static int32_t xauth_init(struct xauth *self, const char *xAuthorityFile) {
    self->fileOffset = 0;

    self->fd = sys_openat(-1, xAuthorityFile, O_RDONLY, 0);
    if (self->fd < 0) return -1;

    // Find file size.
    struct statx statx;
    statx.stx_size = 0;
    int32_t status = sys_statx(self->fd, "", AT_EMPTY_PATH, STATX_SIZE, &statx);
    if (status < 0) {
        status = -2;
        goto cleanup_fd;
    }
    self->fileSize = (int64_t)statx.stx_size;

    if (self->fileSize < 1) {
        status = -3;
        goto cleanup_fd;
    }

    // Map file into memory.
    self->mappedFile = sys_mmap(NULL, self->fileSize, PROT_READ, MAP_PRIVATE, self->fd, 0);
    if ((int64_t)self->mappedFile < 0) {
        status = -4;
        goto cleanup_fd;
    }
    return 0;

    cleanup_fd:
    debug_CHECK(sys_close(self->fd), RES == 0);
    return status;
}

static int32_t xauth_readUint16(struct xauth *self, int64_t *offset) {
    int64_t newOffset = *offset + 2;
    if (newOffset > self->fileSize) return -1;
    int32_t value = ((int32_t)self->mappedFile[*offset] << 8) + (int32_t)self->mappedFile[*offset + 1];
    *offset = newOffset;
    return value;
}

static int32_t xauth_nextEntry(struct xauth *self, struct xauth_entry *entry) {
    int64_t offset = self->fileOffset;

    offset += 2; // Skip family.
    int32_t len = xauth_readUint16(self, &offset); // Address length.
    if (len < 0) return -1;
    offset += len;
    len = xauth_readUint16(self, &offset); // Number length.
    if (len < 0) return -2;
    offset += len;
    // Read name.
    int32_t nameLength = xauth_readUint16(self, &offset);
    if (nameLength < 0) return -3;
    uint8_t *name = &self->mappedFile[offset];
    offset += nameLength;
    // Read data.
    int32_t dataLength = xauth_readUint16(self, &offset);
    if (dataLength < 0) return -4;
    uint8_t *data = &self->mappedFile[offset];
    offset += dataLength;

    if (offset > self->fileSize) return -5;
    // Only touch entry if successful.
    self->fileOffset = offset;
    entry->nameLength = (uint16_t)nameLength;
    entry->name = name;
    entry->dataLength = (uint16_t)dataLength;
    entry->data = data;
    return 0;
}

static void xauth_deinit(struct xauth *self) {
    debug_CHECK(sys_munmap(self->mappedFile, self->fileSize), RES == 0);
    debug_CHECK(sys_close(self->fd), RES == 0);
}
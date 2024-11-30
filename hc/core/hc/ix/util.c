typedef int32_t util_STREAM_T;
#define util_STDIN 0
#define util_STDOUT 1
#define util_STDERR 2

static noreturn void util_abort(void) {
    abort();
}

static int32_t util_writeAll(int32_t fd, const void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        int64_t written = write(fd, buffer + offset, remaining);
        if (written < 0) {
            if (ix_ERRNO(written) == EINTR) continue;
            return -1;
        }
        remaining -= written;
    }
    return 0;
}

static int64_t util_read(int32_t fd, void *buffer, int64_t size) {
    for (;;) {
        int64_t numRead = read(fd, buffer, size);
        if (numRead >= 0) return numRead;
        if (ix_ERRNO(numRead) != EINTR) return -1;
    }
}

// Read `size` bytes, or until EOF. Returns number of bytes read.
static int64_t util_readAll(int32_t fd, void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        int64_t numRead = read(fd, buffer + offset, remaining);
        if (numRead <= 0) {
            if (numRead == 0) return offset;
            if (ix_ERRNO(numRead) == EINTR) continue;
            return -1;
        }
        remaining -= numRead;
    }
    return size;
}

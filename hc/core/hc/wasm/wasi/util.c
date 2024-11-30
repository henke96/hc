typedef int32_t util_STREAM_T;
#define util_STDIN 0
#define util_STDOUT 1
#define util_STDERR 2

static noreturn void util_abort(void) {
    hc_TRAP;
}

static int32_t util_writeAll(int32_t fd, const void *buffer, ssize_t size) {
    ssize_t remaining = size;
    while (remaining > 0) {
        ssize_t offset = size - remaining;
        struct ciovec iov = { buffer + offset, remaining };
        ssize_t written;
        uint16_t err = fd_write(fd, &iov, 1, &written);
        if (err) return -1;
        remaining -= written;
    }
    return 0;
}

static ssize_t util_read(int32_t fd, void *buffer, ssize_t size) {
    struct iovec iov = { buffer, size };
    ssize_t numRead;
    uint16_t err = fd_read(fd, &iov, 1, &numRead);
    if (err) return -1;
    return numRead;
}

// Read `size` bytes, or until EOF. Returns number of bytes read.
static ssize_t util_readAll(int32_t fd, void *buffer, ssize_t size) {
    ssize_t remaining = size;
    while (remaining > 0) {
        ssize_t offset = size - remaining;
        struct iovec iov = { buffer + offset, remaining };
        ssize_t numRead;
        uint16_t err = fd_read(fd, &iov, 1, &numRead);
        if (err) return -1;
        if (numRead == 0) return offset;
        remaining -= numRead;
    }
    return size;
}

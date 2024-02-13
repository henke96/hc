hc_UNUSED
static int32_t util_writeAll(int32_t fd, const void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        int64_t written = write(fd, buffer + offset, remaining);
        if (written < 0) {
            if (written == -EINTR) continue;
            return -1;
        }
        remaining -= written;
    }
    return 0;
}

hc_UNUSED
static int32_t util_readAll(int32_t fd, void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        int64_t numRead = read(fd, buffer + offset, remaining);
        if (numRead <= 0) {
            if (numRead == -EINTR) continue;
            return -1;
        }
        remaining -= numRead;
    }
    return 0;
}

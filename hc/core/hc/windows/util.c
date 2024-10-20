#define util_MAX_UTF8_PER_UTF16 3

hc_UNUSED
static int32_t util_writeAll(void *fileHandle, const void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        uint32_t toWrite = UINT32_MAX;
        if (remaining < UINT32_MAX) toWrite = (uint32_t)remaining;

        uint32_t written;
        if (WriteFile(fileHandle, buffer + offset, toWrite, &written, NULL) == 0) return -1;
        remaining -= written;
    }
    return 0;
}

hc_UNUSED
// Read `size` bytes, or until EOF. Returns number of bytes read.
static int64_t util_readAll(void *fileHandle, void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        uint32_t toRead = UINT32_MAX;
        if (remaining < UINT32_MAX) toRead = (uint32_t)remaining;
        uint32_t numRead;
        if (ReadFile(fileHandle, buffer + offset, toRead, &numRead, NULL) == 0) return -1;
        if (numRead == 0) return offset;
        remaining -= numRead;
    }
    return size;
}

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

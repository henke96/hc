static int32_t fileFd;

static int32_t init(char *file) {
    fileFd = openat(AT_FDCWD, file, O_RDONLY, 0);
    return fileFd;
}

static void deinit(void) {
    debug_CHECK(close(fileFd), RES == 0);
}

static int32_t readIntoBuffer(void) {
    return (int32_t)read(fileFd, &buffer[0], sizeof(buffer));
}

static int32_t printBuffer(int32_t size) {
    return util_writeAll(1, &buffer[0], size);
}

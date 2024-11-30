static int32_t openStream(char *file, util_STREAM_T *stream) {
    int32_t fd = openat(AT_FDCWD, file, O_RDONLY, 0);
    *stream = fd;
    return fd;
}

static void closeStream(util_STREAM_T stream) {
    debug_CHECK(close(stream), RES == 0);
}

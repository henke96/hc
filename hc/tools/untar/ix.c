static int32_t inputFd;
static int32_t fileFd;

static int32_t openInput(char *path) {
    if (path[0] == '-' && path[1] == '\0') {
        inputFd = 0;
    } else {
        inputFd = openat(AT_FDCWD, path, O_RDONLY, 0);
    }
    return inputFd;
}

static int32_t readInput(void) {
    if (util_readAll(inputFd, &buffer[0], sizeof(buffer)) != sizeof(buffer)) return -1;
    return 0;
}

static void closeInput(void) {
    debug_CHECK(close(inputFd), RES == 0);
}

static int32_t createDir(char *prefix, char *name) {
    int32_t prefixFd = AT_FDCWD;
    if (prefix[0] != '\0') {
        prefixFd = openat(AT_FDCWD, prefix, O_RDONLY, 0);
        if (prefixFd < 0) return -1;
    }
    int32_t status = mkdirat(prefixFd, name, 0777);
    if (prefixFd != AT_FDCWD) debug_CHECK(close(prefixFd), RES == 0);
    return status;
}

static int32_t createFile(char *prefix, char *name) {
    int32_t prefixFd = AT_FDCWD;
    if (prefix[0] != '\0') {
        prefixFd = openat(AT_FDCWD, prefix, O_RDONLY, 0);
        if (prefixFd < 0) return -1;
    }
    fileFd = openat(prefixFd, name, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777);
    if (prefixFd != AT_FDCWD) debug_CHECK(close(prefixFd), RES == 0);
    return fileFd;
}

static int32_t writeToFile(int32_t size) {
    return util_writeAll(fileFd, &buffer[0], size);
}

static void closeFile(void) {
    debug_CHECK(close(fileFd), RES == 0);
}

hc_UNUSED
static char *util_getEnv(char **envp, const char *name) {
    for (; *envp != NULL; ++envp) {
        char *env = *envp;
        for (const char *c = name; *c != '\0'; ++c) {
            if (*env == '\0' || *env != *c) goto noMatch;
            ++env;
        }
        if (*env == '=') return env + 1;
        noMatch:;
    }
    return NULL;
}

hc_UNUSED
static uint64_t *util_getAuxv(char **envp) {
    for (; *envp != NULL; ++envp);
    return (uint64_t *)(envp + 1);
}

hc_UNUSED
static int32_t util_getPageSize(const uint64_t *auxv) {
    for (int32_t i = 0;; i += 2) {
        if (auxv[i] == AT_PAGESZ) {
            debug_ASSERT(auxv[i + 1] <= 65536);
            return (int32_t)auxv[i + 1];
        }
        debug_ASSERT(auxv[i] != AT_NULL);
    }
}

hc_UNUSED
static int32_t util_writeAll(int32_t fd, const void *buffer, int64_t size) {
    int64_t remaining = size;
    while (remaining > 0) {
        int64_t offset = size - remaining;
        int64_t written = sys_write(fd, buffer + offset, remaining);
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
        int64_t numRead = sys_read(fd, buffer + offset, remaining);
        if (numRead <= 0) {
            if (numRead == -EINTR) continue;
            return -1;
        }
        remaining -= numRead;
    }
    return 0;
}

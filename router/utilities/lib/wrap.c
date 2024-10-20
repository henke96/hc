static int32_t wrap_getParentPath(char *path, int32_t levelsUp) {
    int32_t pathLen = sys_readlinkat(-1, "/proc/self/exe", path, PATH_MAX);
    if (pathLen >= PATH_MAX) return -1;
    path[pathLen] = '\0';
    for (int32_t i = 0; i < levelsUp; ++i) {
        for (;;) {
            if (pathLen <= 0) return -1;
            if (path[--pathLen] == '/') break;
        }
    }
    return pathLen;
}

static int32_t wrap_appendPath(char *path, int32_t *pathLen, char *append, int32_t appendLen) {
    if (*pathLen + appendLen > PATH_MAX) return -1;
    hc_MEMCPY(&path[*pathLen], append, (uint64_t)appendLen);
    *pathLen += appendLen;
    return 0;
}

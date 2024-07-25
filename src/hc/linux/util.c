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
// Best effort implementation.
static int32_t util_getCpuCount(void) {
    uint64_t mask[16] = {0};
    if (sys_sched_getaffinity(0, sizeof(mask), &mask[0]) < 0) return 1;

    int32_t count = 0;
    for (int32_t i = 0; i < (int32_t)hc_ARRAY_LEN(mask); ++i) {
        count += hc_POPCOUNT64(mask[i]);
    }
    if (count == 0) return 1;
    return count;
}

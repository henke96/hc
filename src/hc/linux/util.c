hc_UNUSED
static inline char **util_getEnvp(int32_t argc, char **argv) {
    return argv + argc + 1;
}

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

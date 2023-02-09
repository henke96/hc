// Should not return.
int32_t __libc_start_main(
    void *main,
    int32_t argc,
    char **argv,
    void (*init)(void),
    void (*fini)(void),
    void (*rtld_fini)(void)
);
int32_t __cxa_atexit(void (*func)(void *), void *arg, void *dso);

int32_t printf(const char *restrict fmt, ...);
int32_t clock_gettime(int32_t clockid, struct timespec *time);

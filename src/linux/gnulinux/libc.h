// Should not return.
int32_t __libc_start_main(
    int32_t (*main)(int32_t argc, char **argv, char **envp),
    int32_t argc,
    char **argv,
    void (*init)(void),
    void (*fini)(void),
    void (*rtld_fini)(void)
);
int __libc_start_main(
    int (*main)(int argc, char **argv, char **envp),
    int argc,
    char **argv,
    void (*init)(void),
    void (*fini)(void),
    void (*rtld_fini)(void)
) {
    return 0;
}
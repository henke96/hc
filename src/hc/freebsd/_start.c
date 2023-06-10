int32_t start(int32_t argc, char **argv, char **envp);

hc_ELF_EXPORT char **environ;
hc_ELF_EXPORT char *__progname = "";

#if hc_X86_64
void _start(char **args, void (*cleanup)(void)) {
    int32_t argc = *(int32_t *)args;
    char **argv = &args[1];
    char **envp = &args[2 + argc];
    __libc_start1(argc, argv, envp, cleanup, start);
}
#elif hc_AARCH64 || hc_RISCV64
void _start(int32_t argc, char **argv, char **envp, void (*cleanup)(void)) {
    __libc_start1(argc, argv, envp, cleanup, start);
}
#endif

hc_ELF_EXPORT char **environ;
hc_ELF_EXPORT char *__progname = "";

#if hc_X86_64
void _start(char **ap, void (*cleanup)(void)) {
    int32_t argc = *(int32_t *)ap;
    char **argv = &ap[1];
    char **env = &ap[2 + argc];
    __libc_start1(argc, argv, env, cleanup, start);
}
#elif hc_AARCH64 || hc_RISCV64
void _start(int32_t argc, char **argv, char **env, void (*cleanup)(void)) {
    __libc_start1(argc, argv, env, cleanup, start);
}
#endif


static int32_t gnuMain(int32_t argc, char **argv, char **envp);

int32_t main(int32_t argc, char **argv) {
    if (argc < 1) return 1; // First argument should be program itself.

    char **envp = util_getEnvp(argc, argv);

    // Check if we have re-executed ourself through the dynamic linker yet (stage2).
    int64_t envpCount = 0;
    for (char **current = envp; *current != NULL; ++current) {
        if (util_cstrCmp(*current, "STAGE2=1") == 0) {
            // Yep, run `__libc_start_main` like a normal C program would.
            __libc_start_main(gnuMain, argc, argv, NULL, NULL, NULL);
            return 1;
        }
        ++envpCount;
    }
    // Nope, run ourself through dynamic linker.

    // Allocate space for new argv and envp, each is 1 longer than old.
    int64_t newArgvCount = (int64_t)argc + 1;
    int64_t newEnvpCount = envpCount + 1;
    int64_t allocSize = ((newArgvCount + 1) + (newEnvpCount + 1)) * (int64_t)sizeof(char *);
    const char **newArgv = sys_mmap(NULL, allocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)newArgv < 0) return 1;
    const char **newEnvp = &newArgv[newArgvCount + 1];

    // Populate new argv and envp.
    for (int64_t i = 0; i < argc; ++i) {
        newArgv[i + 1] = argv[i];
    }
    newArgv[newArgvCount] = NULL;

    newEnvp[0] = "STAGE2=1";
    for (int64_t i = 0; i < envpCount; ++i) {
        newEnvp[i + 1] = envp[i];
    }
    newArgv[newEnvpCount] = NULL;

    // Try known dynamic linker paths.
#if hc_X86_64
    newArgv[0] = "/lib64/ld-linux-x86-64.so.2";
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
    newArgv[0] = "/lib/ld-musl-x86_64.so.1";
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
#elif hc_AARCH64
    newArgv[0] = "/lib/ld-linux-aarch64.so.1";
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
    newArgv[0] = "/lib/ld-musl-aarch64.so.1";
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
#elif hc_RISCV64
    newArgv[0] = "/lib/ld-linux-riscv64-lp64d.so.1"; // Hard floats.
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
    newArgv[0] = "/lib/ld-linux-riscv64-lp64.so.1";
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
    newArgv[0] = "/lib/ld-musl-riscv64.so.1";
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
#endif
    // None of them worked.
    return 1;
}
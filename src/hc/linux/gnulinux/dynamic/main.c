// The main function called after re-running through dynamic linker.
static int32_t libcMain(int32_t argc, char **argv, char **envp);

int32_t main(int32_t argc, char **argv) {
    if (argc < 1) return 1; // First argument should be program itself.

    char **envp = util_getEnvp(argc, argv);

    // Check if we have re-executed ourself through the dynamic linker yet (stage2).
    int64_t envpCount = 0;
    for (char **current = envp; *current != NULL; ++current) {
        if (util_cstrCmp(*current, "STAGE2=1") == 0) {
            // Yep, run `__libc_start_main` like a normal C program would.
            __libc_start_main(libcMain, argc, argv, NULL, NULL, NULL);
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

    // Find dynamic linker path from /usr/bin/env.
    int32_t fd = sys_openat(-1, "/usr/bin/env", O_RDONLY | O_CLOEXEC, 0);
    if (fd < 0) return 1;

    // Map /usr/bin/env into memory.
    struct statx statx;
    statx.stx_size = 0;
    if (sys_statx(fd, "", AT_EMPTY_PATH, STATX_SIZE, &statx) < 0) return 1;
    struct elf_header *elfHeader = sys_mmap(NULL, (int64_t)statx.stx_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ((int64_t)elfHeader < 0) return 1;

    // Find the interpreter program header.
    struct elf_programHeader *programHeaders = (void *)elfHeader + elfHeader->programHeadersOffset;
    for (int32_t i = 0; i < elfHeader->programHeadersLength; ++i) {
        if (programHeaders[i].type == elf_PT_INTERP) {
            // Run ourself through the dynamic linker.
            newArgv[0] = (char *)elfHeader + programHeaders[i].fileOffset;
            sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
            return 1;
        }
    }
    return 1;
}
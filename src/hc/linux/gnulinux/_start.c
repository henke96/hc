#define _start_FUNC "_startGnu"
#include "hc/linux/helpers/_start.c"

hc_WEAK
int32_t __libc_start_main(
    void *main,
    int32_t argc,
    char **argv,
    void (*init)(void),
    void (*fini)(void),
    void (*rtld_fini)(void)
);

int32_t _startGnu(int32_t argc, char **argv) {
    if (argc < 1) return 1;

    // Check if we have re-executed ourself through the dynamic linker yet.
    if (__libc_start_main != 0) __libc_start_main(start, argc, argv, NULL, NULL, NULL); // Yep, hand over to libc.

    // Allocate space for new argv.
    int64_t newArgvCount = (int64_t)argc + 1; // We need an extra argument (argv[0] for dynamic linker).
    int64_t allocSize = (newArgvCount + 1) * (int64_t)sizeof(char *);
    const char **newArgv = sys_mmap(NULL, allocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)newArgv < 0) return 1;

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
    for (int32_t headerIdx = 0; headerIdx < elfHeader->programHeadersLength; ++headerIdx) {
        if (programHeaders[headerIdx].type == elf_PT_INTERP) {
            // Run ourself through the dynamic linker.
            newArgv[0] = (char *)elfHeader + programHeaders[headerIdx].fileOffset;
            // Forward arguments.
            for (int64_t i = 0; i < argc; ++i) {
                newArgv[i + 1] = argv[i];
            }
            newArgv[newArgvCount] = NULL;

            char **envp = util_getEnvp(argc, argv);
            sys_execveat(-1, newArgv[0], &newArgv[0], (const char *const *)&envp[0], 0);
            return 1;
        }
    }
    return 1;
}

int32_t atexit(void (*func)(void)) {
    return __cxa_atexit((void *)func, NULL, NULL);
}

#define _start_FUNC "_startGnu"
#include "hc/linux/helpers/_start.c"

int32_t _startGnu(int32_t argc, char **argv) {
    if (argc < 1) return 1;
    char **envp = util_getEnvp(argc, argv);

    // Check if we have re-executed ourself through the dynamic linker yet (stage2).
    if (util_cstrCmp(argv[argc - 1], "STAGE2=1") == 0) {
        argv[argc - 1] = NULL;
        // Yep, run `__libc_start_main` like a normal C program would.
        __libc_start_main(start, argc - 1, argv, NULL, NULL, NULL);
        return 1;
    }
    // Nope, run ourself through dynamic linker.

    // Allocate space for new argv.
    int64_t newArgvCount = (int64_t)argc + 2;
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
            newArgv[newArgvCount - 1] = "STAGE2=1";
            newArgv[newArgvCount] = NULL;

            sys_execveat(-1, newArgv[0], &newArgv[0], (const char **)&envp[0], 0);
            return 1;
        }
    }
    return 1;
}

int32_t atexit(void (*func)(void)) {
    return __cxa_atexit((void *)func, NULL, NULL);
}


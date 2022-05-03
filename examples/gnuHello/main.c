#include "../../src/hc.h"
#include "../../src/util.c"
#include "../../src/libc/small.c"
#include "../../src/linux/linux.h"
#include "../../src/linux/util.c"
#include "../../src/linux/sys.c"
#include "../../src/linux/helpers/_start.c"
#include "../../src/linux/gnulinux/libdl.h"

static void *libcHandle;

static noreturn void stage1(int32_t argc, char **argv) {
    char **envp = util_getEnvp(argc, argv);
    uint64_t *auxv = util_getAuxv(envp);

    // Find program path.
    char *programPath;
    for (uint64_t *current = auxv; current[0] != AT_NULL; current += 2) {
        if (current[0] == AT_EXECFN) {
            programPath = (char *)current[1];
            goto foundProgramPath;
        }
    }
    // Not found.
    sys_exit(1);

    foundProgramPath:;
    const char *newArgv[3];
    newArgv[1] = programPath;
    newArgv[2] = NULL;
    const char *newEnvp[] = {
        "STAGE2=1",
        NULL
    };
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
    sys_exit(1);
}

static int32_t libcMain(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    int32_t (*printf)(const char *restrict format, ...) = dlsym(libcHandle, "printf");
    if (dlerror() != NULL) return 1;
    printf("Hello world!\n");
    return 0;
}

int32_t main(int32_t argc, char **argv) {
    char **envp = util_getEnvp(argc, argv);
    // Check if we have re-executed ourself through the dynamic linker yet (stage2).
    for (char **current = envp; *current != NULL; ++current) {
        if (util_cstrCmp(*current, "STAGE2=1") == 0) goto stage2; // Yep.
    }
    // Nope.
    stage1(argc, argv);
    hc_UNREACHABLE;

    stage2:;
    libcHandle = dlopen("libc.so.6", RTLD_NOW);
    if (dlerror() != NULL) return 1;

    void (*__libc_start_main)(
        void *main,
        int32_t argc,
        char **argv,
        void *init,
        void *fini,
        void *rtldFini
    ) = dlsym(libcHandle, "__libc_start_main");
    if (dlerror() != NULL) return 1;

    __libc_start_main(libcMain, argc, argv, NULL, NULL, NULL);
    return 1;
}
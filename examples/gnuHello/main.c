#include "../../src/hc.h"
#include "../../src/util.c"
#include "../../src/libc/small.c"
#include "../../src/linux/linux.h"
#include "../../src/linux/util.c"
#include "../../src/linux/sys.c"
#include "../../src/linux/helpers/_start.c"
#include "../../src/linux/gnulinux/libdl.h"

static noreturn void stage1(uint64_t *auxv) {
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
    const char *argv[3];
    argv[1] = programPath;
    argv[2] = NULL;
    const char *envp[] = {
        "STAGE2=1",
        NULL
    };
    // Try known dynamic linker paths.
#if hc_X86_64
    argv[0] = "/lib64/ld-linux-x86-64.so.2";
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
    argv[0] = "/lib/ld-musl-x86_64.so.1";
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
#elif hc_AARCH64
    argv[0] = "/lib/ld-linux-aarch64.so.1";
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
    argv[0] = "/lib/ld-musl-aarch64.so.1";
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
#elif hc_RISCV64
    argv[0] = "/lib/ld-linux-riscv64-lp64d.so.1"; // Hard floats.
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
    argv[0] = "/lib/ld-linux-riscv64-lp64.so.1";
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
    argv[0] = "/lib/ld-musl-riscv64.so.1";
    sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
#endif
    sys_exit(1);
}

int32_t main(int32_t argc, char **argv) {
    char **envp = util_getEnvp(argc, argv);
    uint64_t *auxv = util_getAuxv(envp);

    // Check if we have re-executed ourself through the dynamic linker yet (stage2).
    for (char **current = envp; *current != NULL; ++current) {
        if (util_cstrCmp(*current, "STAGE2=1") == 0) goto stage2; // Yep.
    }
    // Nope.
    stage1(auxv);
    hc_UNREACHABLE;

    stage2:;
    void *handle = dlopen("libc.so.6", RTLD_NOW);
    char *error = dlerror();
    if (error != NULL) {
        sys_writev(STDOUT_FILENO, (struct iovec[2]) {
            { .iov_base = &error[0],    .iov_len = util_cstrLen(error) },
            { .iov_base = (char *)"\n", .iov_len = 1 }
        }, 2);
        return 1;
    }
    int32_t (*printf)(const char *restrict format, ...) = dlsym(handle, "printf");
    error = dlerror();
    if (error != NULL) {
        sys_writev(STDOUT_FILENO, (struct iovec[2]) {
            { .iov_base = &error[0],    .iov_len = util_cstrLen(error) },
            { .iov_base = (char *)"\n", .iov_len = 1 }
        }, 2);
        return 1;
    }
    printf("Hello world!\n");
    return 0;
}
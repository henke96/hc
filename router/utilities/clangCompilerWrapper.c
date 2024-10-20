#include "hc/hc.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"

#include "lib/wrap.c"

int32_t start(int32_t argc, char **argv, char **envp) {
    char path[PATH_MAX];
    int32_t pathLen = wrap_getParentPath(&path[0], 2);
    if (pathLen < 0) return 1;

    int64_t newArgvCount = (int64_t)argc + 2;
    int64_t allocSize = (newArgvCount + 1) * (int64_t)sizeof(char *);
    const char **newArgv = sys_mmap(NULL, allocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)newArgv < 0) return 1;

    newArgv[0] = argv[0];
    newArgv[1] = "-Wno-unused-command-line-argument";
    char dynamicLinkerArg[hc_STR_LEN("-Wl,-dynamic-linker=") + PATH_MAX + hc_STR_LEN("/lib/libc.so\0")];
    char *pos = &dynamicLinkerArg[sizeof(dynamicLinkerArg)];
    pos = hc_MEMCPY(pos - hc_STR_LEN("/lib/libc.so\0"), hc_STR_COMMA_LEN("/lib/libc.so\0"));
    pos = hc_MEMCPY(pos - pathLen, &path[0], (uint64_t)pathLen);
    pos = hc_MEMCPY(pos - hc_STR_LEN("-Wl,-dynamic-linker="), hc_STR_COMMA_LEN("-Wl,-dynamic-linker="));
    newArgv[2] = pos;
    for (int64_t i = 1; i < argc; ++i) {
        newArgv[i + 2] = argv[i];
    }
    newArgv[newArgvCount] = NULL;

    if (wrap_appendPath(&path[0], &pathLen, hc_STR_COMMA_LEN("/bin/clang\0")) < 0) return 1;
    sys_execveat(-1, &path[0], (const char *const *)newArgv, (const char *const *)envp, 0);
    return 1;
}

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

    char **newArgv = argv;

    // Try to determine if we are linking or not.
    bool hasInputFile = false;
    for (int32_t i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'c':
                case 'E': {
                    goto notLinking;
                }
                case '\0': {
                    hasInputFile = true;
                    break;
                }
            }
        } else hasInputFile = true;
    }
    if (!hasInputFile) goto notLinking;

    // We seem to be linking, add the -Wl,-dynamic-linker argument.
    int64_t newArgvCount = (int64_t)argc + 1;
    int64_t allocSize = (newArgvCount + 1) * (int64_t)sizeof(char *);
    newArgv = sys_mmap(NULL, allocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)newArgv < 0) return 1;

    newArgv[0] = argv[0];
    char dynamicLinkerArg[hc_STR_LEN("-Wl,-dynamic-linker=") + PATH_MAX + hc_STR_LEN("/lib/libc.so\0")];
    char *pos = hc_ARRAY_END(dynamicLinkerArg);
    hc_PREPEND_STR(pos, "/lib/libc.so\0");
    hc_PREPEND(pos, &path[0], (uint32_t)pathLen);
    hc_PREPEND_STR(pos, "-Wl,-dynamic-linker=");
    newArgv[1] = pos;
    for (int32_t i = 1; i < argc; ++i) {
        newArgv[i + 1] = argv[i];
    }
    newArgv[newArgvCount] = NULL;

    notLinking:
    if (wrap_appendPath(&path[0], &pathLen, hc_STR_COMMA_LEN("/bin/clang\0")) < 0) return 1;
    sys_execveat(-1, &path[0], (const char *const *)newArgv, (const char *const *)envp, 0);
    return 1;
}

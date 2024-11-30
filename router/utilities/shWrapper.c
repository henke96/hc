#include "hc/hc.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"

#include "lib/wrap.c"

int32_t start(int32_t argc, char **argv, char **envp) {
    char path[PATH_MAX];
    int32_t pathLen = wrap_getParentPath(&path[0], 1);
    if (pathLen < 0) return 1;

    int64_t newArgvCount = (int64_t)argc + 3;
    int64_t allocSize = (newArgvCount + 1) * (int64_t)sizeof(char *);
    const char **newArgv = sys_mmap(NULL, allocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)newArgv < 0) return 1;

    newArgv[0] = &path[0];
    newArgv[1] = "-c";
    char shCommandArg[PATH_MAX + hc_STR_LEN(" \"$@\"\0")];
    char *pos = hc_ARRAY_END(shCommandArg);
    hc_PREPEND_STR(pos, " \"$@\"\0");
    char *command = &path[pathLen + 1];
    uint64_t commandLen = (uint64_t)util_cstrLen(command);
    hc_PREPEND(pos, command, commandLen);
    newArgv[2] = pos;
    for (int64_t i = 0; i < argc; ++i) {
        newArgv[i + 3] = argv[i];
    }
    newArgv[newArgvCount] = NULL;

    if (wrap_appendPath(&path[0], &pathLen, hc_STR_COMMA_LEN("/sh\0")) < 0) return 1;
    sys_execveat(-1, newArgv[0], (const char *const *)newArgv, (const char *const *)envp, 0);
    return 1;
}

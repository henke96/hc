#include "hc/hc.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"

#include "lib/wrap.c"

int32_t start(hc_UNUSED int32_t argc, char **argv, char **envp) {
    char path[PATH_MAX];
    int32_t pathLen = wrap_getParentPath(&path[0], 1);
    if (pathLen < 0) return 1;

    if (wrap_appendPath(&path[0], &pathLen, hc_STR_COMMA_LEN("/clang\0")) < 0) return 1;
    sys_execveat(-1, &path[0], (const char *const *)argv, (const char *const *)envp, 0);
    return 1;
}

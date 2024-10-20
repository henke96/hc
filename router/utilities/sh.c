#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/helpers/_start.c"

int32_t start(hc_UNUSED int32_t argc, char **argv, char **envp) {
    char *path = util_getEnv(envp, "PATH");
    if (path == NULL) return 1;

    for(;;) {
        char buffer[PATH_MAX];
        int32_t prefixLen = 0;
        for (;; ++prefixLen) {
            char c = path[prefixLen];
            if (c == '\0' || c == ':') {
                buffer[prefixLen] = '\0';
                break;
            }
            if (prefixLen >= (int32_t)sizeof(buffer) - 1) return 1;
            buffer[prefixLen] = c;
        }
        int32_t prefixFd = sys_openat(-1, &buffer[0], O_RDONLY | O_CLOEXEC, 0);
        if (prefixFd > 0) {
            sys_execveat(prefixFd, "sh", (const char *const *)argv, (const char *const *)envp, 0);
            debug_CHECK(sys_close(prefixFd), RES == 0);
        }
        if (path[prefixLen] == '\0') return 1;
        path += prefixLen + 1;
    }
}

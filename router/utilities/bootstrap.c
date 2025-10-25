#include "hc/hc.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
static noreturn void abort(void) {
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
}
#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"
#include "hc/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/helpers/_start.c"

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    if (argc != 2) {
        debug_print("Usage: bootstrap DEVICE|DIRECTORY\n");
        return 1;
    }

    char *device = argv[1];
    char *path = "/mnt";

    struct statx statx;
    statx.stx_mode = 0; // Make static analysis happy.
    if (sys_statx(-1, device, 0, STATX_TYPE, &statx) < 0) return 1;
    if (S_ISDIR(statx.stx_mode)) {
        path = device;
        device = NULL;
    } else {
        if (sys_umount2(path, 0) == 0) return 0;
    }

    uint64_t pathLen = (uint64_t)util_cstrLen(path);
    if (pathLen > (PATH_MAX - hc_STR_LEN("/downloads\0"))) return 1;

    if (sys_setsid() < 0) return 1;
    if (sys_ioctl(0, TIOCSCTTY, 0) < 0) return 1;
    if (device != NULL && sys_mount(device, path, "iso9660", MS_RDONLY, NULL) < 0) return 1;
    if (sys_chdir(path) < 0) return 1;

    static char buffer[
        hc_STR_LEN("PATH=\0") + PATH_MAX +
        hc_STR_LEN("DOWNLOADS=\0") + PATH_MAX +
        hc_STR_LEN("PARALLEL=\0") + util_UINT64_MAX_CHARS +
        PATH_MAX
    ];
    char *pos = hc_ARRAY_END(buffer);
    hc_PREPEND_STR(pos, "/bin\0");
    hc_PREPEND(pos, path, pathLen);
    hc_PREPEND_STR(pos, "PATH=");
    char *pathEnv = pos;
    hc_PREPEND_STR(pos, "/downloads\0");
    hc_PREPEND(pos, path, pathLen);
    hc_PREPEND_STR(pos, "DOWNLOADS=");
    char *downloadsEnv = pos;
    hc_PREPEND_STR(pos, "\0");
    pos = util_uintToStr(pos, (uint64_t)util_getCpuCount());
    hc_PREPEND_STR(pos, "PARALLEL=");
    char *numCpusEnv = pos;
    hc_PREPEND_STR(pos, "/bin/sh\0");
    hc_PREPEND(pos, path, pathLen);
    char *shPath = pos;
    const char *shArgv[] = { shPath, NULL };
    const char *shEnvp[] = { "HOME=/", "TERM=linux", pathEnv, downloadsEnv, numCpusEnv, NULL };
    sys_execveat(-1, shArgv[0], &shArgv[0], &shEnvp[0], 0);
    return 1;
}

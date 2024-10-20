#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/helpers/_start.c"

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    if (argc != 2) {
        sys_write(2, hc_STR_COMMA_LEN("Usage: bootstrap DEVICE|DIRECTORY\n"));
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
    }

    int64_t pathLen = util_cstrLen(path);
    if (pathLen > (PATH_MAX - (int64_t)hc_STR_LEN("/downloads\0"))) return 1;

    if (sys_setsid() < 0) return 1;
    if (sys_ioctl(0, TIOCSCTTY, 0) < 0) return 1;
    if (device != NULL && sys_mount(device, path, "iso9660", MS_RDONLY, NULL) < 0) return 1;

    static char buffer[
        hc_STR_LEN("PATH=\0") + PATH_MAX +
        hc_STR_LEN("DOWNLOADS=\0") + PATH_MAX +
        hc_STR_LEN("NUM_CPUS=\0") + util_UINT64_MAX_CHARS +
        PATH_MAX
    ];
    char *pos = &buffer[sizeof(buffer)];
    pos = hc_MEMCPY(pos - hc_STR_LEN("/bin\0"), hc_STR_COMMA_LEN("/bin\0"));
    pos = hc_MEMCPY(pos - pathLen, path, (uint64_t)pathLen);
    pos = hc_MEMCPY(pos - hc_STR_LEN("PATH="), hc_STR_COMMA_LEN("PATH="));
    char *pathEnv = pos;
    pos = hc_MEMCPY(pos - hc_STR_LEN("/downloads\0"), hc_STR_COMMA_LEN("/downloads\0"));
    pos = hc_MEMCPY(pos - pathLen, path, (uint64_t)pathLen);
    pos = hc_MEMCPY(pos - hc_STR_LEN("DOWNLOADS="), hc_STR_COMMA_LEN("DOWNLOADS="));
    char *downloadsEnv = pos;
    *--pos = '\0';
    pos = util_uintToStr(pos, (uint64_t)util_getCpuCount());
    pos = hc_MEMCPY(pos - hc_STR_LEN("NUM_CPUS="), hc_STR_COMMA_LEN("NUM_CPUS="));
    char *numCpusEnv = pos;
    pos = hc_MEMCPY(pos - hc_STR_LEN("/bin/sh\0"), hc_STR_COMMA_LEN("/bin/sh\0"));
    pos = hc_MEMCPY(pos - pathLen, path, (uint64_t)pathLen);
    char *shPath = pos;
    const char *shArgv[] = { shPath, NULL };
    const char *shEnvp[] = { "HOME=/", "TERM=linux", pathEnv, downloadsEnv, numCpusEnv, NULL };
    sys_execveat(-1, shArgv[0], &shArgv[0], &shEnvp[0], 0);
    return 1;
}

#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/heap.c"
#include "hc/linux/helpers/_start.c"
static int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"
#include "hc/tar.h"

#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"

#include "../common.c"

static void initPageSize(char **envp) {
    pageSize = util_getPageSize(util_getAuxv(envp));
}
#define openat sys_openat
#define close sys_close
static int32_t fstatat(int32_t fd, const char *path, struct stat *stat, uint32_t flags) {
    struct statx statx;
    statx.stx_mode = 0; // Make static analysis happy.
    statx.stx_size = 0; // Make static analysis happy.
    if (sys_statx(fd, path, flags, STATX_TYPE | STATX_SIZE, &statx) < 0) return -1;
    stat->st_mode = statx.stx_mode;
    stat->st_size = statx.stx_size;
    return 0;
}
#define ix_D_NAME(DIRENT) ((void *)&(DIRENT)->d_name)
#define dirent linux_dirent64
#define getdents sys_getdents64
#include "../ix.c"

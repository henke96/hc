#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
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
#include "hc/linux/heap.c"
#include "hc/linux/helpers/_start.c"
int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"

#include "../common.c"
#define openat sys_openat
#define close sys_close
static int32_t fstatat(int32_t fd, const char *path, struct stat *stat, uint32_t flags) {
    struct statx statx;
    statx.stx_size = 0; // Make static analysis happy.
    if (sys_statx(fd, path, flags, STATX_SIZE, &statx) < 0) return -1;
    stat->st_size = statx.stx_size;
    return 0;
}
#include "../ix.c"

static void initPageSize(char **envp) {
    pageSize = util_getPageSize(util_getAuxv(envp));
}

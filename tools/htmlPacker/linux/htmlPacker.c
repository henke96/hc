#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/heap.c"
#include "hc/linux/helpers/_start.c"
int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"

#include "../common.c"
#define openat sys_openat
#define read sys_read
#define close sys_close
struct stat {
    struct statx inner;
};
#define fstatat(FD, PATH, STAT, FLAGS) sys_statx(FD, PATH, FLAGS, STATX_SIZE, &(STAT)->inner)
#define st_size inner.stx_size
#include "../ix.c"

static void initPageSize(char **envp) {
    pageSize = util_getPageSize(util_getAuxv(envp));
}

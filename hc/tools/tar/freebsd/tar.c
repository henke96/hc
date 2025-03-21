#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#define ix_ERRNO(RET) errno
#include "hc/ix/util.c"
#include "hc/debug.c"
#include "hc/freebsd/heap.c"
#include "hc/freebsd/_start.c"
static int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"
#include "hc/tar.h"
#include "hc/argParse.h"

#include "../common.c"

static void initPageSize(hc_UNUSED char **envp) {
    debug_CHECK(elf_aux_info(AT_PAGESZ, &pageSize, sizeof(pageSize)), RES == 0);
}

#define ix_D_NAME(DIRENT) ((void *)&(DIRENT)[1])
#include "../ix.c"

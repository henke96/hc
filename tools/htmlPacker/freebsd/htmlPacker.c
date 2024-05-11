#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#include "hc/freebsd/debug.c"
#include "hc/freebsd/util.c"
#include "hc/freebsd/heap.c"
#include "hc/freebsd/_start.c"
static int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"

#include "../common.c"
#include "../ix.c"

static void initPageSize(hc_UNUSED char **envp) {
    debug_CHECK(elf_aux_info(AT_PAGESZ, &pageSize, sizeof(pageSize)), RES == 0);
}

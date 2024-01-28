#include "hc/hc.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/windows/windows.h"
#include "hc/windows/util.c"
#include "hc/windows/debug.c"
#include "hc/windows/heap.c"

static struct SYSTEMINFO systemInfo;
#define allocator_PAGE_SIZE systemInfo.pageSize
#include "hc/allocator.c"

#include "../test.c"

void noreturn _start(void) {
    GetSystemInfo(&systemInfo);

    int32_t status = test();
    debug_printNum("Status: ", status, "\n");
    ExitProcess(0);
}

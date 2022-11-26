#include "hc/hc.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/libc/small.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"
#include "hc/windows/heap.c"

#include "hc/allocator.c"

#include "../test.c"

void noreturn _start(void) {
    int32_t status = test();
    debug_printNum("Status: ", status, "\n");
    ExitProcess(0);
}
#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/libc/small.c"
#include "hc/linux/linux.h"
#include "hc/linux/util.c"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/heap.c"
#include "hc/linux/helpers/_start.c"

#include "hc/allocator.c"

#include "../test.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    int32_t status = test();
    debug_printNum("Status: ", status, "\n");
    return 0;
}

#include "hc/hc.h"
#include "hc/math.c"
#include "hc/debug.h"
#include "hc/wasm/heap.c"

#include "debug.c"
#define allocator_PAGE_SIZE 65536
#include "hc/allocator.c"

#include "../test.c"

hc_EXPORT void start(void) {
    int32_t status = test();
    debug_printNum("Status: ", status, "\n");
}

#include "hc/hc.h"
#include "hc/math.c"
#include "hc/wasm/heap.c"

#include "debug.h"
#include "hc/wasm/debug.c"
#define allocator_PAGE_SIZE 65536
#include "hc/allocator.c"

#include "../test.c"

hc_WASM_EXPORT("start")
void start(void) {
    int32_t status = test();
    debug_printNum("Status: ", status, "\n");
}

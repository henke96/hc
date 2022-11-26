#include "hc/hc.h"
#include "hc/math.c"
#include "hc/wasm/heap.c"

#include "debug.h"
#include "hc/allocator.c"

#include "../test.c"

hc_WASM_EXPORT("_start")
void _start(void) {
    int32_t status = test();
    debug_printNum("Status: ", status, "\n");
}
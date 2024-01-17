#include "hc/hc.h"
#include "hc/jni.h"
#include "hc/debug.h"
#include "hc/windows/windows.h"
#include "hc/util.c"
#include "hc/windows/util.c"
#include "hc/windows/debug.c"

#include "../common.c"

int32_t _start(hc_UNUSED void *handle, hc_UNUSED uint32_t reason, hc_UNUSED void *reserved) {
    return 1;
}

#include "hc/hc.h"
#include "hc/linux/android/android.h"
#include "hc/linux/android/liblog.so.h"

void ANativeActivity_onCreate(hc_UNUSED void* activity, hc_UNUSED void *savedState, hc_UNUSED uint64_t savedStateSize) {
    __android_log_write(android_LOG_INFO, "androidapp", "Hello!\n");
}

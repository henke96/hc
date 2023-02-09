#include "hc/hc.h"
#include "hc/egl.h"
#include "hc/util.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/android/liblog.so.h"
#include "hc/linux/android/libdl.so.h"
#define egl_SO_NAME "libEGL.so"
#include "hc/linux/egl.c"

void ANativeActivity_onCreate(hc_UNUSED void* activity, hc_UNUSED void *savedState, hc_UNUSED uint64_t savedStateSize) {
    struct egl egl;
    int32_t status = egl_init(&egl);
    if (status < 0) {
        return;
    }
    const int32_t configAttributes[] = {
        egl_RED_SIZE, 8,
        egl_GREEN_SIZE, 8,
        egl_BLUE_SIZE, 8,
        egl_DEPTH_SIZE, 24,
        egl_STENCIL_SIZE, 8,
        egl_NONE
    };
    const int32_t contextAttributes[] = {
        egl_CONTEXT_MAJOR_VERSION, 3,
        egl_CONTEXT_MINOR_VERSION, 0,
        egl_NONE
    };
    status = egl_createContext(&egl, egl_OPENGL_ES_API, &configAttributes[0], &contextAttributes[0]);
    if (status < 0) {
        return;
    }

    __android_log_write(android_LOG_INFO, "androidApp", "Hello!\n");
}

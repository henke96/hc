#include "hc/hc.h"
#include "hc/egl.h"
#include "hc/gl.h"
#include "hc/util.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/android/android.h"
#include "hc/linux/android/liblog.so.h"
#include "hc/linux/android/libdl.so.h"
#define egl_SO_NAME "libEGL.so"
#include "hc/linux/egl.c"

static void (*glClear)(uint32_t mask);
static void (*glClearColor)(float red, float green, float blue, float alpha);

static void onWindowCreated(hc_UNUSED struct ANativeActivity *activity, void *window) {
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

    status = egl_setupSurface(&egl, window);
    if (status < 0) {
        return;
    }

    if ((glClear = egl_getProcAddress(&egl, "glClear")) == NULL) return;
    if ((glClearColor = egl_getProcAddress(&egl, "glClearColor")) == NULL) return;

    glClearColor(0.5f, 0.7f, 0.3f, 1.0f);
    glClear(gl_COLOR_BUFFER_BIT);

    if (!egl_swapBuffers(&egl)) return;

    __android_log_write(android_LOG_INFO, "androidApp", "Success!\n");
}

void ANativeActivity_onCreate(struct ANativeActivity *activity, hc_UNUSED void *savedState, hc_UNUSED uint64_t savedStateSize) {
    activity->callbacks->onNativeWindowCreated = onWindowCreated;
    __android_log_write(android_LOG_INFO, "androidApp", "Hello!\n");
}

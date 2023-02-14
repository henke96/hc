#include "hc/hc.h"
#include "hc/egl.h"
#include "hc/gl.h"
#include "hc/util.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/sys_clone_func.c"
#include "hc/linux/android/android.h"
#include "hc/linux/android/liblog.so.h"
#include "hc/linux/android/libdl.so.h"
#include "hc/linux/android/debug.c"
#define egl_SO_NAME "libEGL.so"
#include "hc/linux/egl.c"

static void (*glClear)(uint32_t mask);
static void (*glClearColor)(float red, float green, float blue, float alpha);

static char threadStack[8096] hc_ALIGNED(16);
static int32_t threadDone;

static inline void resetThreadDone(void) {
    hc_ATOMIC_STORE(&threadDone, 0, hc_ATOMIC_RELEASE);
}

static void waitThreadDone(void) {
    for (;;) {
        if (hc_ATOMIC_LOAD(&threadDone, hc_ATOMIC_ACQUIRE) == 1) break;
        debug_CHECK(sys_futex(&threadDone, FUTEX_WAIT_PRIVATE, 0, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
    }
}

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

    debug_print("Success!");
}

static noreturn void thread(hc_UNUSED void *arg) {
    debug_print("Hello from thread!\n");
    hc_ATOMIC_STORE(&threadDone, 1, hc_ATOMIC_RELEASE);
    debug_CHECK(sys_futex(&threadDone, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0), RES >= 0);
    sys_exit(0);
}

void ANativeActivity_onCreate(struct ANativeActivity *activity, hc_UNUSED void *savedState, hc_UNUSED uint64_t savedStateSize) {
    activity->callbacks->onNativeWindowCreated = onWindowCreated;
    debug_print("Hello!\n");

    resetThreadDone();
    uint32_t flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM;
    int32_t ret = sys_clone_func(flags, &threadStack[sizeof(threadStack)], NULL, 0, NULL, thread, NULL);
    if (ret < 0) {
        debug_printNum("Failed to create thread (", ret, ")\n");
        return;
    }
    waitThreadDone();
    debug_print("Thread started!\n");
}

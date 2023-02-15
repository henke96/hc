#include "hc/hc.h"
#include "hc/egl.h"
#include "hc/gl.h"
#include "hc/util.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/sys_clone_func.c"
#include "hc/linux/android/android.h"
#include "hc/linux/android/libc.so.h"
#include "hc/linux/android/libandroid.so.h"
#include "hc/linux/android/liblog.so.h"
#include "hc/linux/android/libdl.so.h"
#include "hc/linux/android/debug.c"
#include "hc/linux/android/nativeGlue.c"
#define egl_SO_NAME "libEGL.so"
#include "hc/linux/egl.c"

static void (*glClear)(uint32_t mask);
static void (*glClearColor)(float red, float green, float blue, float alpha);

struct app {
    struct egl egl;
    void *window;
    void *inputQueue;
};

static struct app app;

static void app_init(void) {
    app.window = NULL;
    app.inputQueue = NULL;
}

static int32_t app_initEgl(void) {
    // No error recovery as we end up aborting anyway.
    int32_t status = egl_init(&app.egl);
    if (status != 0) {
        debug_printNum("Failed to initialise EGL (", status, ")\n");
        return -1;
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
    status = egl_createContext(&app.egl, egl_OPENGL_ES_API, &configAttributes[0], &contextAttributes[0]);
    if (status < 0) {
        debug_printNum("Failed to create EGL context (", status, ")\n");
        return -1;
    }

    status = egl_setupSurface(&app.egl, app.window);
    if (status != 0) {
        debug_printNum("Failed to setup EGL surface (", status, ")\n");
        return -1;
    }

    if ((glClear = egl_getProcAddress(&app.egl, "glClear")) == NULL) return -1;
    if ((glClearColor = egl_getProcAddress(&app.egl, "glClearColor")) == NULL) return -1;
    return 0;
}

#define INPUT_QUEUE_LOOPER_ID 1

static int32_t appThread(void *looper, hc_UNUSED void *arg) {
    app_init();

    // Main loop.
    for (;;) {
        // Handle all events.
        for (;;) {
            int32_t fd;
            int32_t timeout = 0;
            if (app.window == NULL) timeout = -1; // Wait indefinitely.
            int32_t ident = ALooper_pollAll(timeout, &fd, NULL, NULL);
            if (ident == ALOOPER_POLL_TIMEOUT) break;
            if (ident < 0) return -1;

            if (ident == nativeGlue_LOOPER_ID) {
                struct nativeGlue_cmd cmd;
                cmd.tag = nativeGlue_NO_CMD;
                int64_t ret = sys_read(fd, &cmd, sizeof(cmd));
                if (ret != sizeof(cmd)) return -2;

                switch (cmd.tag) {
                    case nativeGlue_NATIVE_WINDOW_CREATED: {
                        debug_ASSERT(app.window == NULL);
                        app.window = cmd.nativeWindowCreated.window;
                        if (app_initEgl() != 0) return -3;
                        break;
                    }
                    case nativeGlue_NATIVE_WINDOW_DESTROYED: {
                        debug_ASSERT(app.window == cmd.nativeWindowDestroyed.window);
                        egl_deinit(&app.egl);
                        app.window = NULL;
                        break;
                    }
                    case nativeGlue_INPUT_QUEUE_CREATED: {
                        debug_ASSERT(app.inputQueue == NULL);
                        app.inputQueue = cmd.inputQueueCreated.inputQueue;
                        AInputQueue_attachLooper(app.inputQueue, looper, INPUT_QUEUE_LOOPER_ID, NULL, NULL);
                        break;
                    }
                    case nativeGlue_INPUT_QUEUE_DESTROYED: {
                        debug_ASSERT(app.inputQueue == cmd.inputQueueDestroyed.inputQueue);
                        AInputQueue_detachLooper(app.inputQueue);
                        app.inputQueue = NULL;
                        break;
                    }
                    case nativeGlue_DESTROY: {
                        if (app.inputQueue != NULL) AInputQueue_detachLooper(app.inputQueue);
                        return 0;
                    }
                    default: break;
                }
                nativeGlue_signalAppDone();

            } else if (ident == INPUT_QUEUE_LOOPER_ID) {
                void *inputEvent;
                while (AInputQueue_getEvent(app.inputQueue, &inputEvent) >= 0) {
                    if (AInputQueue_preDispatchEvent(app.inputQueue, inputEvent) != 0) continue;
                    AInputQueue_finishEvent(app.inputQueue, inputEvent, 1);
                }
            }
        }
        if (app.window == NULL) continue;

        // Render.
        glClearColor(0.5f, 0.7f, 0.3f, 1.0f);
        glClear(gl_COLOR_BUFFER_BIT);
        if (egl_swapBuffers(&app.egl) != 1) return -4;
    }
}

void ANativeActivity_onCreate(struct ANativeActivity *activity, hc_UNUSED void *savedState, hc_UNUSED uint64_t savedStateSize) {
    struct nativeGlue_appInfo appInfo = {
        .appThreadFunc = appThread,
        .appThreadArg = NULL
    };
    if (nativeGlue_init(activity, &appInfo) != 0) debug_abort();
}

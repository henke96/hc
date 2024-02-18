#include "hc/hc.h"
#include "hc/egl.h"
#include "hc/gl.h"
#include "hc/jni.h"
#include "hc/util.c"
#include "hc/debug.h"
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
#include "hc/linux/egl.c"

// TODO: Implement input
hc_UNUSED static void game_onKeyDown(int32_t key, uint64_t timestamp);
hc_UNUSED static void game_onKeyUp(int32_t key, uint64_t timestamp);
#define game_EXPORT static
#define gl_GET_PROC_ADDR(LOADER_PTR, FUNC) egl_getProcAddress(LOADER_PTR, FUNC)
#include "../shared/gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"

struct app {
    struct egl egl;
    void *window;
    void *inputQueue;
    int32_t width;
    int32_t height;
    bool running;
    char __pad[7];
};

static struct app app;

static int32_t app_init(void) {
    app.window = NULL;
    app.inputQueue = NULL;
    app.width = 0;
    app.height = 0;
    app.running = false;

    int32_t status = egl_init(&app.egl, "libEGL.so");
    if (status < 0) {
        debug_printNum("Failed to initalise EGL (", status, ")\n");
        return -1;
    }
    return 0;
}

static int32_t app_initEgl(void) {
    // No error recovery as we end up aborting anyway.
    const int32_t configAttributes[] = {
        egl_BUFFER_SIZE, 32,
        egl_RED_SIZE, 8,
        egl_GREEN_SIZE, 8,
        egl_BLUE_SIZE, 8,
        egl_ALPHA_SIZE, 8,
        egl_DEPTH_SIZE, 24,
        egl_STENCIL_SIZE, 8,
        egl_NONE
    };
    const int32_t contextAttributes[] = {
        egl_CONTEXT_MAJOR_VERSION, 3,
        egl_CONTEXT_MINOR_VERSION, 0,
        egl_NONE
    };
    int32_t status = egl_createContext(
        &app.egl,
        -1,
        NULL,
        egl_DEFAULT_DISPLAY,
        egl_OPENGL_ES_API,
        &configAttributes[0],
        &contextAttributes[0]
    );
    if (status < 0) {
        debug_printNum("Failed to initialise EGL context (", status, ")\n");
        return -1;
    }

    status = egl_createSurface(&app.egl, app.window);
    if (status != 0) {
        debug_printNum("Failed to setup EGL surface (", status, ")\n");
        return -1;
    }

    debug_CHECK(egl_swapInterval(&app.egl, 1), RES == egl_TRUE); // TODO: Replace vsync with choreographer.

    if (
        egl_querySurface(&app.egl, egl_WIDTH, &app.width) != 1 ||
        egl_querySurface(&app.egl, egl_HEIGHT, &app.height) != 1
    ) {
        debug_print("Failed to query EGL surface\n");
        return -1;
    }

    if (gl_init(&app.egl) != 0) {
        debug_print("Failed to load OpenGL functions\n");
        return -1;
    }

    return 0;
}

#define INPUT_QUEUE_LOOPER_ID 1

static int32_t appThread(void *looper, hc_UNUSED void *arg) {
    if (app_init() != 0) return -1;

    // Main loop.
    for (;;) {
        // Handle all events.
        for (;;) {
            int32_t fd;
            int32_t timeout = app.running ? 0 : -1;
            int32_t ident = ALooper_pollAll(timeout, &fd, NULL, NULL);
            if (ident == ALOOPER_POLL_TIMEOUT) break;
            if (ident < 0) return -2;

            if (ident == nativeGlue_LOOPER_ID) {
                struct nativeGlue_cmd cmd;
                cmd.tag = nativeGlue_NO_CMD;
                int64_t ret = sys_read(fd, &cmd, sizeof(cmd));
                if (ret != sizeof(cmd)) return -3;

                switch (cmd.tag) {
                    case nativeGlue_WINDOW_FOCUS_CHANGED: {
                        if (cmd.windowFocusChanged.hasFocus) *cmd.windowFocusChanged.requestPointerCapture = true;
                        break;
                    }
                    case nativeGlue_NATIVE_WINDOW_CREATED: {
                        debug_ASSERT(app.window == NULL);
                        app.window = cmd.nativeWindowCreated.window;
                        if (app_initEgl() != 0) return -4;

                        // Initialise game.
                        struct timespec initTimespec;
                        debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &initTimespec), RES == 0);
                        uint64_t initTimestamp = (uint64_t)initTimespec.tv_sec * 1000000000 + (uint64_t)initTimespec.tv_nsec;
                        int32_t status = game_init(
                            app.width,
                            app.height,
                            initTimestamp
                        );
                        if (status < 0) {
                            debug_printNum("Failed to initialise game (", status, ")\n");
                            return -5;
                        }
                        app.running = true;
                        break;
                    }
                    // TODO: Should separate game logic from rendering so we don't restart the game every time app is stopped.
                    case nativeGlue_NATIVE_WINDOW_DESTROYED:
                    case nativeGlue_STOP: {
                        if (app.running) {
                            game_deinit();
                            app.running = false;
                        }
                        if (cmd.tag == nativeGlue_NATIVE_WINDOW_DESTROYED) {
                            debug_ASSERT(app.window != NULL);
                            egl_destroySurface(&app.egl);
                            egl_destroyContext(&app.egl);
                            app.window = NULL;
                        }
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
                        egl_deinit(&app.egl);
                        return 0;
                    }
                    default: break;
                }
                nativeGlue_signalAppDone();

            } else if (ident == INPUT_QUEUE_LOOPER_ID) {
                void *inputEvent;
                while (AInputQueue_getEvent(app.inputQueue, &inputEvent) >= 0) {
                    if (AInputQueue_preDispatchEvent(app.inputQueue, inputEvent) != 0) continue;
                    int32_t handled = 0;
                    if (app.running) {
                        int32_t type = AInputEvent_getType(inputEvent);
                        int32_t source = AInputEvent_getSource(inputEvent);
                        if (type == AINPUT_EVENT_TYPE_MOTION && source == AINPUT_SOURCE_MOUSE_RELATIVE) {
                            int32_t action = AMotionEvent_getAction(inputEvent);
                            if (action == AMOTION_EVENT_ACTION_MOVE) {
                                int64_t dXTemp = (int64_t)((double)AMotionEvent_getRawX(inputEvent, 0) * 65536.0);
                                int64_t dYTemp = (int64_t)((double)AMotionEvent_getRawY(inputEvent, 0) * 65536.0);
                                uint64_t dX = (uint64_t)dXTemp * 65536;
                                uint64_t dY = (uint64_t)dYTemp * 65536;
                                game_onMouseMove((int64_t)dX, (int64_t)dY, 0);
                                handled = 1;
                            }
                        }
                    }
                    AInputQueue_finishEvent(app.inputQueue, inputEvent, handled);
                }
            }
        }
        if (!app.running) continue;

        // Check for resize before every render.
        int32_t width, height;
        if (
            egl_querySurface(&app.egl, egl_WIDTH, &width) == 1 &&
            egl_querySurface(&app.egl, egl_HEIGHT, &height) == 1 &&
            (width != app.width || height != app.height)
        ) {
            app.width = width;
            app.height = height;
            game_onResize(app.width, app.height);
        }

        // Rendering.
        struct timespec drawTimespec;
        debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &drawTimespec), RES == 0);
        uint64_t drawTimestamp = (uint64_t)drawTimespec.tv_sec * 1000000000 + (uint64_t)drawTimespec.tv_nsec;
        if (game_draw(drawTimestamp) < 0) return -6;
        debug_CHECK(egl_swapBuffers(&app.egl), RES == 1);
    }
}

hc_EXPORT
void ANativeActivity_onCreate(struct ANativeActivity *activity, hc_UNUSED void *savedState, hc_UNUSED uint64_t savedStateSize) {
    struct nativeGlue_appInfo appInfo = {
        .appThreadFunc = appThread,
        .appThreadArg = NULL
    };
    if (nativeGlue_init(activity, &appInfo) != 0) debug_abort();
}

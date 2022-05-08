#include "../../src/hc.h"
#include "../../src/util.c"
#include "../../src/libc/small.c"
#include "../../src/linux/linux.h"
#include "../../src/linux/util.c"
#include "../../src/linux/sys.c"
#include "../../src/linux/helpers/_start.c"
#include "../../src/linux/gnulinux/x11.h"
#include "../../src/linux/gnulinux/dynamic/libc.so.6.h"
#include "../../src/linux/gnulinux/dynamic/libdl.so.2.h"
#include "../../src/linux/gnulinux/dynamic/egl.h"
#include "../../src/linux/gnulinux/dynamic/egl.c"
#include "../../src/linux/gnulinux/dynamic/main.c"

#include "x11Client.c"

static int32_t (*printf)(const char *restrict format, ...);

// Returns window id, or negative error code.
static int64_t openX11Window(struct x11Client *x11Client, uint32_t visualId) {
    // Use first screen.
    uint64_t rootsOffset = util_ALIGN_FORWARD(x11Client->setupResponse->vendorLength, 4) + sizeof(struct x11_format) * x11Client->setupResponse->numPixmapFormats;
    struct x11_screen *screen = (void *)&x11Client->setupResponse->data[rootsOffset];
    uint32_t parentId = screen->windowId;

    struct requests {
        struct x11_createWindow createWindow;
        struct x11_mapWindow mapWindow;
        struct x11_getWindowAttributes getWindowAttrs;
    };

    struct requests windowRequests = {
        .createWindow = {
            .opcode = x11_createWindow_OPCODE,
            .depth = 0,
            .length = sizeof(windowRequests.createWindow) / 4,
            .windowId = x11Client_nextId(x11Client),
            .parentId = parentId,
            .x = 100,
            .y = 100,
            .width = 200,
            .height = 200,
            .borderWidth = 1,
            .class = x11_INPUT_OUTPUT,
            .visualId = visualId,
            .valueMask = 0
        },
        .mapWindow = {
            .opcode = x11_mapWindow_OPCODE,
            .length = sizeof(windowRequests.mapWindow) / 4,
            .windowId = windowRequests.createWindow.windowId
        },
        .getWindowAttrs = {
            .opcode = x11_getWindowAttributes_OPCODE,
            .length = sizeof(windowRequests.getWindowAttrs) / 4,
            .windowId = windowRequests.createWindow.windowId
        }
    };

    if (x11Client_sendRequests(x11Client, &windowRequests, sizeof(windowRequests), 3) < 0) return -1;

    // Read response to getWindowAttributes request.
    // We do this to catch errors with window creation, and to wait for window to be ready.
    struct x11_getWindowAttributesReponse response;
    int64_t numRead = sys_recvfrom(x11Client->socketFd, &response, sizeof(struct x11_getWindowAttributesReponse), 0, NULL, NULL);
    if (numRead <= 0) return -2;
    if (response.type != x11_TYPE_REPLY) return -3;

    // Read remaining response.
    while (numRead < (int64_t)sizeof(response)) {
        char *readPos = &((char *)&response)[numRead];
        int64_t read = sys_recvfrom(x11Client->socketFd, readPos, (int64_t)sizeof(response) - numRead, 0, NULL, NULL);
        if (read <= 0) return -4;
        numRead += read;
    }
    return windowRequests.createWindow.windowId;
}

static int32_t libcMain(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    void *libcHandle = dlopen("libc.so.6", RTLD_NOW);
    if (dlerror() != NULL) return 1;

    printf = dlsym(libcHandle, "printf");
    if (dlerror() != NULL) return 1;

    struct x11Client x11Client;
    int32_t status = x11Client_init(&x11Client);
    if (status < 0) {
        printf("Failed to initialise x11Client (%d)\n", status);
        return 1;
    }

    struct egl egl;
    status = egl_init(&egl);
    if (status < 0) {
        printf("Failed to initalise EGL (%d)\n", status);
        return 1;
    }
    const int32_t configAttributes[] = {
        egl_RED_SIZE, 8,
        egl_GREEN_SIZE, 8,
        egl_BLUE_SIZE, 8,
        egl_NONE
    };
    const int32_t contextAttributes[] = {
        egl_CONTEXT_MAJOR_VERSION, 3,
        egl_CONTEXT_MINOR_VERSION, 2,
        egl_NONE
    };
    status = egl_createContext(&egl, egl_OPENGL_API, &configAttributes[0], &contextAttributes[0]);
    if (status < 0) {
        printf("Failed to create EGL context (%d)\n", status);
        return 1;
    }
    uint32_t eglVisualId = (uint32_t)status;

    int64_t windowId = openX11Window(&x11Client, eglVisualId);
    if (status < 0) {
        printf("Failed to create X11 window (%d)\n", status);
        return 1;
    }

    status = egl_setupSurface(&egl, (uint32_t)windowId);
    if (status < 0) {
        printf("Failed to setup EGL surface (%d)\n", status);
        return 1;
    }

    // Do some drawing.
    void *glHandle = dlopen("libGL.so.1", RTLD_NOW);
    if (dlerror() != NULL) return 1;
    void (*glClear)(uint32_t mask) = dlsym(glHandle, "glClear");
    if (dlerror() != NULL) return 1;
    void (*glClearColor)(float red, float green, float blue, float alpha) = dlsym(glHandle, "glClearColor");
    if (dlerror() != NULL) return 1;

    for (;;) {
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(0x00004000);
        if (!egl_swapBuffers(&egl)) return 1;
    }

    egl_deinit(&egl);
    x11Client_deinit(&x11Client);
    return 0;
}

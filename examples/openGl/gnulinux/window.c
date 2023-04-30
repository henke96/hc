enum window_platform {
    window_X11,
    window_GBM
};

struct window_x11 {
    struct x11Client client;
    struct x11_getKeyboardMappingResponse *keyboardMap;
    struct x11_getModifierMappingResponse *modifierMap;
    uint32_t keyboardMapSize;
    uint32_t windowId;
    uint32_t rootWindowId;
    uint32_t wmProtocolsAtom;
    uint32_t wmDeleteWindowAtom;
    uint32_t wmStateAtom;
    uint32_t wmStateFullscreenAtom;
    uint32_t wmBypassCompositorAtom;
    uint32_t motifWmHintsAtom;
    uint8_t xinputMajorOpcode;
    uint8_t xfixesMajorOpcode;
    char __pad[2];
};

struct window_gbm {
    struct gbm gbm;
    struct drmKms drmKms;
    void *gbmSurface;
    uint32_t gbmFormat;
    int32_t drmModeIndex;
};

struct window {
    struct egl egl;
    int32_t epollFd;
    enum window_platform platform;
    uint16_t width;
    uint16_t height;
    bool pointerGrabbed;
    char __pad[3];
    union {
        struct window_x11 x11;
        struct window_gbm gbm;
    };
};

static struct window window;

static const int32_t window_configAttributes[] = {
    egl_BUFFER_SIZE, 32,
    egl_RED_SIZE, 8,
    egl_GREEN_SIZE, 8,
    egl_BLUE_SIZE, 8,
    egl_ALPHA_SIZE, 8,
    egl_DEPTH_SIZE, 24,
    egl_STENCIL_SIZE, 8,
    egl_NONE
};
static const int32_t window_contextAttributes[] = {
    egl_CONTEXT_MAJOR_VERSION, 3,
    egl_CONTEXT_MINOR_VERSION, 0,
    egl_NONE
};
static const char *window_platforms[] = {
    [window_X11] = "X11",
    [window_GBM] = "GBM"
};

#include "window_x11.c"
#include "window_gbm.c"

static int32_t window_init(char **envp) {
    window.pointerGrabbed = false;
    window.width = 640;
    window.height = 480;

    window.epollFd = sys_epoll_create1(0);
    if (window.epollFd < 0) return -1;

    int32_t status = egl_init(&window.egl, "libEGL.so.1");
    if (status < 0) {
        debug_printNum("Failed to initalise EGL (", status, ")\n");
        goto cleanup_epollFd;
    }

    void *eglWindow;
    for (uint32_t platform = 0; platform < hc_ARRAY_LEN(window_platforms); ++platform) {
        switch (platform) {
            case window_X11: status = window_x11_init(&eglWindow, envp); break;
            case window_GBM: status = window_gbm_init(&eglWindow); break;
            default: hc_UNREACHABLE;
        }
        struct iovec_const print[] = {
            { hc_STR_COMMA_LEN("Using platform ") },
            { window_platforms[platform], util_cstrLen(window_platforms[platform]) },
            { hc_STR_COMMA_LEN("\n") },
        };
        if (status < 0) print[0] = (struct iovec_const) { hc_STR_COMMA_LEN("Failed using platform ") };
        sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
        if (status == 0) {
            window.platform = platform;
            goto initialisedPlatform;
        }
    }
    // Failed all platforms.
    goto cleanup_egl;

    initialisedPlatform:;
    // Create EGL surface.
    status = egl_createSurface(&window.egl, eglWindow);
    if (status < 0) {
        debug_printNum("Failed to create EGL surface (", status, ")\n");
        goto cleanup_platform;
    }

    debug_CHECK(egl_swapInterval(&window.egl, 0), RES == egl_TRUE);

    // Load OpenGL functions.
    status = gl_init(&window.egl);
    if (status < 0) {
        debug_printNum("Failed to initialise GL (", status, ")\n");
        goto cleanup_eglSurface;
    }

    // Initialise game.
    struct timespec initTimespec;
    debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &initTimespec), RES == 0);
    uint64_t initTimestamp = (uint64_t)initTimespec.tv_sec * 1000000000 + (uint64_t)initTimespec.tv_nsec;
    status = game_init(
        window.width,
        window.height,
        initTimestamp
    );
    if (status < 0) {
        debug_printNum("Failed to initialise game (", status, ")\n");
        goto cleanup_eglSurface;
    }
    return 0;

    cleanup_eglSurface:
    egl_destroySurface(&window.egl);
    cleanup_platform:
    switch (window.platform) {
        case window_X11: window_x11_deinit(); break;
        case window_GBM: window_gbm_deinit(); break;
        default: hc_UNREACHABLE;
    }
    cleanup_egl:
    egl_deinit(&window.egl);
    cleanup_epollFd:
    debug_CHECK(sys_close(window.epollFd), RES == 0);
    return -1;
}

static inline int32_t window_run(void) {
    switch (window.platform) {
        case window_X11: return window_x11_run();
        case window_GBM: return window_gbm_run();
        default: hc_UNREACHABLE;
    }
}

static void window_deinit(void) {
    game_deinit();
    egl_destroySurface(&window.egl);
    switch (window.platform) {
        case window_X11: window_x11_deinit(); break;
        case window_GBM: window_gbm_deinit(); break;
        default: hc_UNREACHABLE;
    }
    egl_deinit(&window.egl);
    debug_CHECK(sys_close(window.epollFd), RES == 0);
}

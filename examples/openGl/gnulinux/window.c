enum window_platforms {
    window_X11,
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

struct window {
    struct egl egl;
    int32_t epollFd;
    enum window_platforms platform;
    bool pointerGrabbed;
    char __pad[7];
    union {
        struct window_x11 x11;
    };
};

static struct window window;

#include "window_x11.c"

static int32_t window_init(char **envp) {
    window.pointerGrabbed = false;

    window.epollFd = sys_epoll_create1(0);
    if (window.epollFd < 0) return -1;

    // Platforms.
    const uint32_t eglPlatforms[] = {
        [window_X11] = egl_PLATFORM_XCB_EXT
    };
    const char *platformNames[] = {
        [window_X11] = "X11"
    };

    // Initialise EGL and detect platform.
    int32_t status = egl_init(&window.egl, "libEGL.so.1", &eglPlatforms[0], hc_ARRAY_LEN(eglPlatforms));
    if (status < 0) {
        debug_printNum("Failed to initalise EGL (", status, ")\n");
        goto cleanup_epollFd;
    } else if (status == hc_ARRAY_LEN(eglPlatforms)) {
        status = window_X11; // Guess X11.
    }
    window.platform = (enum window_platforms)status;

    // Print detected platform.
    struct iovec print[] = {
        { hc_STR_COMMA_LEN("Detected platform: ") },
        { (char *)platformNames[window.platform], util_cstrLen(platformNames[window.platform]) },
        { hc_STR_COMMA_LEN("\n") },
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));

    // Create EGL context.
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
    status = egl_createContext(&window.egl, egl_OPENGL_ES_API, &configAttributes[0], &contextAttributes[0]);
    if (status < 0) {
        debug_printNum("Failed to create EGL context (", status, ")\n");
        goto cleanup_egl;
    }
    uint32_t eglVisualId = (uint32_t)status;

    // Initialise platform.
    switch (window.platform) {
        case window_X11: status = window_x11_init(envp, eglVisualId); break;
        default: hc_UNREACHABLE;
    }
    if (status < 0) {
        debug_printNum("Failed to initialise platform (", status, ")\n");
        goto cleanup_egl;
    }
    return 0;

    cleanup_egl:
    egl_deinit(&window.egl);
    cleanup_epollFd:
    debug_CHECK(sys_close(window.epollFd), RES == 0);
    return -1;
}

static inline int32_t window_run(void) {
    switch (window.platform) {
        case window_X11: return window_x11_run();
        default: hc_UNREACHABLE;
    }
}

static void window_deinit(void) {
    switch (window.platform) {
        case window_X11: window_x11_deinit(); break;
        default: hc_UNREACHABLE;
    }
    egl_deinit(&window.egl);
    debug_CHECK(sys_close(window.epollFd), RES == 0);
}

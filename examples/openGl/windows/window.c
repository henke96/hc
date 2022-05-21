struct window {
    struct wgl wgl;
    void *windowHandle;
};

static struct window window;

static int64_t window_proc(
    void *windowHandle,
    uint32_t message,
    uint64_t wParam,
    int64_t lParam
) {
    switch (message) {
        case WM_CREATE: {
            void *dc = GetDC(windowHandle);
            if (dc == NULL) return -1;

            struct PIXELFORMATDESCRIPTOR pfd = {
                .size = sizeof(pfd),
                .version = 1,
                .flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                .pixelType = PFD_TYPE_RGBA,
                .colorBits = 24, // TODO: 24 or 32?
                .depthBits = 24,
                .stencilBits = 8,
                .layerType = PFD_MAIN_PLANE
            };
            int32_t format = ChoosePixelFormat(dc, &pfd);
            if (format == 0 || !SetPixelFormat(dc, format, &pfd)) return -1;

            if (wgl_init(&window.wgl, dc) < 0) return -1;

            // OpenGL 4.3 is compatible with OpenGL ES 3.0.
            int32_t contextAttributes[] = {
                wgl_CONTEXT_MAJOR_VERSION_ARB, 4,
                wgl_CONTEXT_MINOR_VERSION_ARB, 3,
                wgl_CONTEXT_PROFILE_MASK_ARB, wgl_CONTEXT_CORE_PROFILE_BIT_ARB,
                0
            };
            if (wgl_updateContext(&window.wgl, &contextAttributes[0]) < 0) goto cleanup_wgl;
            debug_CHECK(wgl_swapInterval(&window.wgl, 0), == 1);

            if (gl_init(&window.wgl) < 0) goto cleanup_wgl;
            if (game_init() < 0) goto cleanup_wgl;
            return 0;

            cleanup_wgl:
            wgl_deinit(&window.wgl);
            return -1;
        };
        case WM_DESTROY: {
            game_deinit();
            wgl_deinit(&window.wgl);
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            int32_t width = lParam & 0xffff;
            int32_t height = (lParam >> 16) & 0xffff;
            game_resize(width, height);
            return 0;
        }
    }
    return DefWindowProcW(windowHandle, message, wParam, lParam);
}

static int32_t window_init(void) {
    struct WNDCLASSW windowClass = {
        .instanceHandle = __ImageBase,
        .className = u"gl",
        .windowProc = window_proc,
        .style = CS_OWNDC
    };
    if (!RegisterClassW(&windowClass)) return -1;

    void *windowHandle = CreateWindowExW(
        0,
        windowClass.className,
        u"",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        NULL,
        NULL,
        windowClass.instanceHandle,
        NULL
    );
    if (windowHandle == NULL) {
        debug_CHECK(UnregisterClassW(windowClass.className, windowClass.instanceHandle), == 1);
        return -2;
    }
    return 0;
}

static int32_t window_run(void) {
    struct MSG msg;
    for (;;) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return (int32_t)msg.wParam;
            DispatchMessageW(&msg);
        }
        if (game_draw() < 0 || !wgl_swapBuffers(&window.wgl)) debug_CHECK(DestroyWindow(window.windowHandle), != 0);
    }
}

static void window_deinit(void) {
    debug_CHECK(UnregisterClassW(u"gl", __ImageBase), == 1);
}
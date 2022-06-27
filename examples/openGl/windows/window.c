#define window_DEFAULT_WIDTH 640
#define window_DEFAULT_HEIGHT 480

extern void *__ImageBase;

struct window {
    struct wgl wgl;
    void *dc;
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
            struct CREATESTRUCTW *createStruct = (void *)lParam;
            window.dc = GetDC(windowHandle);
            if (window.dc == NULL) return -1;

            int32_t formatAttributes[] = {
                wgl_DRAW_TO_WINDOW_ARB, 1,
                wgl_SUPPORT_OPENGL_ARB, 1,
                wgl_DOUBLE_BUFFER_ARB, 1,
                wgl_ACCELERATION_ARB, wgl_FULL_ACCELERATION_ARB,
                wgl_PIXEL_TYPE_ARB, wgl_TYPE_RGBA_ARB,
                wgl_RED_BITS_ARB, 8,
                wgl_GREEN_BITS_ARB, 8,
                wgl_BLUE_BITS_ARB, 8,
                wgl_DEPTH_BITS_ARB, 24,
                wgl_STENCIL_BITS_ARB, 8,
                0
            };
            // OpenGL 4.3 is compatible with OpenGL ES 3.0.
            int32_t contextAttributes[] = {
                wgl_CONTEXT_MAJOR_VERSION_ARB, 4,
                wgl_CONTEXT_MINOR_VERSION_ARB, 3,
                wgl_CONTEXT_PROFILE_MASK_ARB, wgl_CONTEXT_CORE_PROFILE_BIT_ARB,
                0
            };
            int32_t status = wgl_createContext(&window.wgl, window.dc, &formatAttributes[0], &contextAttributes[0]);
            if (status < 0) {
                debug_printNum("Failed to create context (", status, ")\n");
                goto cleanup_dc;
            }
            debug_CHECK(wgl_swapInterval(&window.wgl, 0), RES == 1);

            if (gl_init(&window.wgl) < 0) goto cleanup_context;

            status = game_init(createStruct->width, createStruct->height);
            if (status < 0) {
                debug_printNum("Failed to initialise game (", status, ")\n");
                goto cleanup_context;
            }
            return 0;

            cleanup_context:
            wgl_destroyContext(&window.wgl, window.dc);
            cleanup_dc:
            debug_CHECK(ReleaseDC(windowHandle, window.dc), RES == 1);
            return -1;
        };
        case WM_DESTROY: {
            game_deinit();
            wgl_destroyContext(&window.wgl, window.dc);
            debug_CHECK(ReleaseDC(windowHandle, window.dc), RES == 1);
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            int32_t width = lParam & 0xffff;
            int32_t height = (lParam >> 16) & 0xffff;
            game_onResize(width, height);
            return 0;
        }
    }
    return DefWindowProcW(windowHandle, message, wParam, lParam);
}

static int32_t window_init(void) {
    if (wgl_init(&window.wgl) < 0) return -1;

    int32_t status;
    struct WNDCLASSW windowClass = {
        .instanceHandle = __ImageBase,
        .className = u"gl",
        .windowProc = window_proc,
        .style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW
    };
    if (!RegisterClassW(&windowClass)) {
        status = -2;
        goto cleanup_wgl;
    }

    window.windowHandle = CreateWindowExW(
        0,
        windowClass.className,
        u"",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, window_DEFAULT_WIDTH, window_DEFAULT_HEIGHT,
        NULL,
        NULL,
        windowClass.instanceHandle,
        NULL
    );
    if (window.windowHandle == NULL) {
        status = -3;
        goto cleanup_windowClass;
    }
    return 0;

    cleanup_windowClass:
    debug_CHECK(UnregisterClassW(windowClass.className, windowClass.instanceHandle), RES == 1);
    cleanup_wgl:
    wgl_deinit(&window.wgl);
    return status;
}

static void window_run(void) {
    int64_t frameCounter = 0;
    int64_t timerFrequency;
    QueryPerformanceFrequency(&timerFrequency);
    int64_t prevTime;
    QueryPerformanceCounter(&prevTime);

    struct MSG msg;
    for (;;) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return;
            DispatchMessageW(&msg);
        }
        if (game_draw() < 0 || !SwapBuffers(window.dc)) {
            debug_CHECK(PostMessageW(window.windowHandle, WM_CLOSE, 0, 0), RES != 0);
            continue;
        }

        ++frameCounter;
        int64_t currentTime;
        QueryPerformanceCounter(&currentTime);
        if (currentTime - prevTime >= timerFrequency) {
            debug_printNum("FPS: ", frameCounter, "\n");
            frameCounter = 0;
            prevTime = currentTime;
        }
    }
}

static void window_deinit(void) {
    debug_CHECK(UnregisterClassW(u"gl", __ImageBase), RES == 1);
    wgl_deinit(&window.wgl);
}
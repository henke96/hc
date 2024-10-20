static int32_t window_codeToKey[0x80] = {
    [0x10] = 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    [0x1E] = 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    [0X2C] = 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
};

extern void *__ImageBase;

struct window {
    struct wgl wgl;
    void *dc;
    void *windowHandle;
    uint64_t timestampMult;
    uint64_t eventTimestamp;
    struct WINDOWPLACEMENT prevPlacement;
    uint16_t width;
    uint16_t height;
    bool pointerGrabbed;
    bool initialised;
    bool fullscreen;
    char __pad[5];
};

static struct window window;

static void _window_clipCursor(void *windowHandle) {
    struct RECT rect;
    if (
        GetClientRect(windowHandle, &rect) &&
        ClientToScreen(windowHandle, (struct POINT *)&rect.left) &&
        ClientToScreen(windowHandle, (struct POINT *)&rect.right)
    ) {
        debug_CHECK(ClipCursor(&rect), RES != 0);
    }
}

static void _window_fixWindowRect(void *windowHandle, struct RECT *rect) {
    int32_t needAdjustMask = WS_MAXIMIZE | WS_BORDER;
    if ((GetWindowLongW(windowHandle, GWL_STYLE) & needAdjustMask) == needAdjustMask) {
        int32_t padding = GetSystemMetrics(SM_CXPADDEDBORDER);
        int32_t xAdjust = GetSystemMetrics(SM_CXSIZEFRAME) + padding;
        int32_t yAdjust = GetSystemMetrics(SM_CYSIZEFRAME) + padding;
        rect->left += xAdjust;
        rect->right -= xAdjust;
        rect->top += yAdjust;
        rect->bottom -= yAdjust;
    }
}

static void _window_draw(void) {
    int64_t drawTime;
    debug_CHECK(QueryPerformanceCounter(&drawTime), RES != 0);
    game_draw((uint64_t)drawTime * window.timestampMult, !window.pointerGrabbed && !window.fullscreen);
    debug_CHECK(SwapBuffers(window.dc), RES == 1);
}

static int64_t _window_proc(
    void *windowHandle,
    uint32_t message,
    uint64_t wParam,
    int64_t lParam
) {
    switch (message) {
        case WM_CREATE: {
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

            int64_t initTime;
            debug_CHECK(QueryPerformanceCounter(&initTime), RES != 0);
            status = game_init((uint64_t)initTime * window.timestampMult);
            if (status < 0) {
                debug_printNum("Failed to initialise game (", status, ")\n");
                goto cleanup_context;
            }
            window.initialised = true;

            // Force a WM_NCCALCSIZE message to remove window decorations.
            SetWindowPos(
                windowHandle, NULL,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
            );
            break;

            cleanup_context:
            wgl_destroyContext(&window.wgl, window.dc);
            cleanup_dc:
            debug_CHECK(ReleaseDC(windowHandle, window.dc), RES == 1);
            return -1;
        }
        case WM_DESTROY: {
            // Windows calls this even if WM_CREATE failed..
            if (window.initialised) {
                game_deinit();
                wgl_destroyContext(&window.wgl, window.dc);
                debug_CHECK(ReleaseDC(windowHandle, window.dc), RES == 1);
                PostQuitMessage(0);
            }
            break;
        }
        case WM_NCCALCSIZE: {
            if (!wParam) break;

            _window_fixWindowRect(windowHandle, (struct RECT *)lParam);
            return 0;
        }
        case WM_NCHITTEST: {
            if (window.pointerGrabbed || window.fullscreen) return HTCLIENT;

            int16_t x = (int16_t)(lParam & 0xFFFF);
            int16_t y = (int16_t)((lParam >> 16) & 0xFFFF);
            struct RECT windowRect;
            debug_CHECK(GetWindowRect(windowHandle, &windowRect), RES != 0);
            _window_fixWindowRect(windowHandle, &windowRect);

            bool left = (x < windowRect.left + 5);
            bool right = (x >= windowRect.right - 5);
            bool top = (y < windowRect.top + 5);
            bool bottom = (y >= windowRect.bottom - 5);
            if (left) {
                if (top) return HTTOPLEFT;
                if (bottom) return HTBOTTOMLEFT;
                return HTLEFT;
            }
            if (right) {
                if (top) return HTTOPRIGHT;
                if (bottom) return HTBOTTOMRIGHT;
                return HTRIGHT;
            }
            if (top) return HTTOP;
            if (bottom) return HTBOTTOM;
            if (y < windowRect.top + game_FRAME_HEIGHT) {
                if (x >= windowRect.right - game_FRAME_HEIGHT) return HTCLOSE;
                return HTCAPTION;
            }
            return HTCLIENT;
        }
        case WM_NCLBUTTONDOWN: {
            // DefWindowProcW seemingly doesn't trust wParam for the close button.
            if (wParam == HTCLOSE) {
                debug_CHECK(PostMessageW(windowHandle, WM_SYSCOMMAND, SC_CLOSE, 0), RES != 0);
                return 0;
            }
            break;
        }
        case WM_SIZE: {
            window.width = (uint16_t)(lParam & 0xFFFF);
            window.height = (uint16_t)((lParam >> 16) & 0xFFFF);
            if (window.pointerGrabbed) _window_clipCursor(windowHandle);
            game_onResize();
            break;
        }
        case WM_KILLFOCUS: {
            if (window.pointerGrabbed) {
                // Ungrab pointer.
                debug_CHECK(ClipCursor(NULL), RES != 0);
                debug_CHECK(ShowCursor(1), RES == 0);
                window.pointerGrabbed = false;
            }
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            if (!window.pointerGrabbed) {
                // Grab pointer.
                _window_clipCursor(windowHandle);
                debug_CHECK(ShowCursor(0), RES == -1);
                window.pointerGrabbed = true;
            }
            break;
        }
        case WM_INPUT: {
            struct RAWINPUT input;
            uint32_t inputSize = sizeof(input);
            if (GetRawInputData((void *)lParam, RID_INPUT, &input, &inputSize, sizeof(struct RAWINPUTHEADER)) == (uint32_t)-1) return -1;

            if (input.header.type == RIM_TYPEMOUSE) {
                if (!window.pointerGrabbed) return 0;

                if (!(input.data.mouse.flags & MOUSE_MOVE_ABSOLUTE)) {
                    int64_t deltaX = (int64_t)((uint64_t)input.data.mouse.lastX << 32);
                    int64_t deltaY = (int64_t)((uint64_t)input.data.mouse.lastY << 32);
                    game_onMouseMove(deltaX, deltaY, window.eventTimestamp);
                }
            } else if (input.header.type == RIM_TYPEKEYBOARD) {
                if ((input.data.keyboard.flags & RI_KEY_E1) != 0 || input.data.keyboard.makeCode > 0x7F) return 0; // Ignore weirdness.

                int32_t key = window_codeToKey[input.data.keyboard.makeCode];
                if (!(input.data.keyboard.flags & RI_KEY_BREAK)) {
                    // Key down.
                    if (key != 0) game_onKeyDown(key, window.eventTimestamp);
                    else if (input.data.keyboard.makeCode == 0x01) { // Escape.
                        if (window.pointerGrabbed) {
                            // Ungrab pointer.
                            debug_CHECK(ClipCursor(NULL), RES != 0);
                            debug_CHECK(ShowCursor(1), RES == 0);
                            window.pointerGrabbed = false;
                        }
                    } else if (input.data.keyboard.makeCode == 0x57) { // F11.
                        // Toggle fullscreen.
                        int32_t style = GetWindowLongW(windowHandle, GWL_STYLE);
                        window.fullscreen = !window.fullscreen;
                        if (window.fullscreen) {
                            struct MONITORINFOEXW mi = { .size = sizeof(mi) };
                            debug_CHECK(GetWindowPlacement(windowHandle, &window.prevPlacement), RES != 0);
                            debug_CHECK(GetMonitorInfoW(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &mi), RES != 0);
                            SetWindowLongW(windowHandle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
                            debug_CHECK(
                                SetWindowPos(
                                    windowHandle,
                                    HWND_TOP,
                                    mi.monitorRect.left, mi.monitorRect.top,
                                    mi.monitorRect.right - mi.monitorRect.left,
                                    mi.monitorRect.bottom - mi.monitorRect.top,
                                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED
                                ),
                                RES != 0
                            );
                        } else {
                            SetWindowLongW(windowHandle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
                            debug_CHECK(
                                SetWindowPlacement(windowHandle, &window.prevPlacement),
                                RES != 0
                            );
                            debug_CHECK(
                                SetWindowPos(
                                    windowHandle, NULL, 0, 0, 0, 0,
                                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
                                ),
                                RES != 0
                            );
                        }
                    }
                } else {
                    // Key up.
                    if (key != 0) game_onKeyUp(key, window.eventTimestamp);
                }
            }
            break;
        }
        case WM_PAINT: {
            _window_draw();
            break;
        }
    }
    return DefWindowProcW(windowHandle, message, wParam, lParam);
}

static int32_t window_init(void) {
    window.initialised = false;
    window.pointerGrabbed = false;
    window.fullscreen = false;
    window.width = 640;
    window.height = 480;
    window.prevPlacement.length = sizeof(window.prevPlacement);

    int64_t timerFrequency;
    if (QueryPerformanceFrequency(&timerFrequency) == 0) return -1;
    if (1000000000 % timerFrequency != 0) return -2;
    window.timestampMult = 1000000000 / (uint64_t)timerFrequency;

    if (wgl_init(&window.wgl) < 0) return -3;

    int32_t status;
    struct WNDCLASSW windowClass = {
        .instanceHandle = __ImageBase,
        .className = u"gl",
        .windowProc = _window_proc,
        .style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW,
        .cursorHandle = LoadCursorW(NULL, IDC_ARROW)
    };
    if (!RegisterClassW(&windowClass)) {
        status = -4;
        goto cleanup_wgl;
    }

    window.windowHandle = CreateWindowExW(
        0,
        windowClass.className,
        u"",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window.width, window.height,
        NULL,
        NULL,
        windowClass.instanceHandle,
        NULL
    );
    if (window.windowHandle == NULL) {
        status = -5;
        goto cleanup_windowClass;
    }

    // Request raw input for mouse and keyboard.
    struct RAWINPUTDEVICE rawDevices[2] = {
        {
            .usagePage = 1,
            .usage = 2, // Mouse.
            .flags = 0, // We still want WM_LBUTTONDOWN etc.
            .targetWindowHandle = NULL
        }, {
            .usagePage = 1,
            .usage = 6, // Keyboard.
            .flags = RIDEV_NOLEGACY,
            .targetWindowHandle = NULL
        }
    };
    if (!RegisterRawInputDevices(&rawDevices[0], 2, sizeof(rawDevices[0]))) {
        status = -6;
        goto cleanup_windowClass;
    }
    return 0;

    cleanup_windowClass:
    debug_CHECK(UnregisterClassW(windowClass.className, windowClass.instanceHandle), RES != 0);
    cleanup_wgl:
    wgl_deinit(&window.wgl);
    return status;
}

static void window_run(void) {
    struct MSG msg;
    for (;;) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            int64_t eventTime;
            debug_CHECK(QueryPerformanceCounter(&eventTime), RES != 0);
            window.eventTimestamp = (uint64_t)eventTime * window.timestampMult;

            if (msg.message == WM_QUIT) return;
            DispatchMessageW(&msg);
        }
        _window_draw();
    }
}

static void window_deinit(void) {
    debug_CHECK(UnregisterClassW(u"gl", __ImageBase), RES != 0);
    wgl_deinit(&window.wgl);
}

static int32_t window_width(void) {
    return window.width;
}

static int32_t window_height(void) {
    return window.height;
}

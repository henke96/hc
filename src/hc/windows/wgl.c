extern void *__ImageBase;

struct wgl {
    void *context;
    void *dlHandle;
    void *(*wglGetProcAddress)(const char *name);
    void *(*wglCreateContext)(void *dc);
    int32_t (*wglMakeCurrent)(void *dc, void *context);
    int32_t (*wglDeleteContext)(void *context);
    void *(*wglCreateContextAttribsARB)(void *dc, void *shareContext, const int32_t *attributes);
    int32_t (*wglChoosePixelFormatARB)(void *dc, const int32_t *iAttributes, const float *fAttributes, uint32_t maxFormats, int32_t *formats, uint32_t *numFormats);
    int32_t (*wglSwapIntervalEXT)(int32_t interval);
};

static int32_t wgl_init(struct wgl *self) {
    self->dlHandle = LoadLibraryW(L"opengl32.dll");
    if (self->dlHandle == NULL) return -1;

    int32_t status = -2;
    if ((self->wglGetProcAddress = GetProcAddress(self->dlHandle, "wglGetProcAddress")) == NULL) goto cleanup_dlHandle;
    if ((self->wglCreateContext = GetProcAddress(self->dlHandle, "wglCreateContext")) == NULL) goto cleanup_dlHandle;
    if ((self->wglMakeCurrent = GetProcAddress(self->dlHandle, "wglMakeCurrent")) == NULL) goto cleanup_dlHandle;
    if ((self->wglDeleteContext = GetProcAddress(self->dlHandle, "wglDeleteContext")) == NULL) goto cleanup_dlHandle;

    // Create dummy window.
    struct WNDCLASSW windowClass = {
        .instanceHandle = __ImageBase,
        .className = L"dummy",
        .windowProc = DefWindowProcW,
        .style = CS_OWNDC
    };
    if (!RegisterClassW(&windowClass)) {
        status = -3;
        goto cleanup_dlHandle;
    }

    void *windowHandle = CreateWindowExW(
        0,
        windowClass.className,
        L"",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        windowClass.instanceHandle,
        NULL
    );
    if (windowHandle == NULL) {
        status = -4;
        goto cleanup_windowClass;
    }

    void *dc = GetDC(windowHandle);
    if (dc == NULL) {
        status = -5;
        goto cleanup_windowHandle;
    }

    struct PIXELFORMATDESCRIPTOR pfd = {
        .size = sizeof(pfd),
        .version = 1,
        .flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .pixelType = PFD_TYPE_RGBA,
        .colorBits = 24,
        .depthBits = 24,
        .stencilBits = 8,
        .layerType = PFD_MAIN_PLANE
    };
    int32_t format = ChoosePixelFormat(dc, &pfd);
    if (format == 0 || !SetPixelFormat(dc, format, &pfd)) {
        status = -6;
        goto cleanup_dc;
    }

    // Create dummy context.
    void *context = self->wglCreateContext(dc);
    if (context == NULL) {
        status = -7;
        goto cleanup_dc;
    }

    if (!self->wglMakeCurrent(dc, context)) {
        status = -8;
        goto cleanup_context;
    }

    // Load extension functions.
    status = -9;
    if ((self->wglCreateContextAttribsARB = self->wglGetProcAddress("wglCreateContextAttribsARB")) == NULL) goto cleanup_context_current;
    if ((self->wglChoosePixelFormatARB = self->wglGetProcAddress("wglChoosePixelFormatARB")) == NULL) goto cleanup_context_current;
    if ((self->wglSwapIntervalEXT = self->wglGetProcAddress("wglSwapIntervalEXT")) == NULL) goto cleanup_context_current;

    status = 0;
    cleanup_context_current:
    debug_CHECK(self->wglMakeCurrent(dc, NULL), RES == 1);
    cleanup_context:
    debug_CHECK(self->wglDeleteContext(context), RES == 1);
    cleanup_dc:
    debug_CHECK(ReleaseDC(windowHandle, dc), RES == 1);
    cleanup_windowHandle:
    debug_CHECK(DestroyWindow(windowHandle), RES != 0);
    cleanup_windowClass:
    debug_CHECK(UnregisterClassW(windowClass.className, windowClass.instanceHandle), RES == 1);
    if (status == 0) return 0; // On success we clean up everything except the library.

    cleanup_dlHandle:
    debug_CHECK(FreeLibrary(self->dlHandle), RES != 0);
    return status;
}

static int32_t wgl_createContext(struct wgl *self, void *dc, const int32_t *formatAttributes, const int32_t *contextAttributes) {
    uint32_t numFormats;
    int32_t format;
    if (!self->wglChoosePixelFormatARB(dc, formatAttributes, NULL, 1, &format, &numFormats) || numFormats != 1) return -1;

    struct PIXELFORMATDESCRIPTOR pfd;
    debug_CHECK(DescribePixelFormat(dc, format, sizeof(pfd), &pfd), RES != 0);
    if (!SetPixelFormat(dc, format, &pfd)) return -2;

    self->context = self->wglCreateContextAttribsARB(dc, NULL, contextAttributes);
    if (self->context == NULL) return -3;
    debug_CHECK(self->wglMakeCurrent(dc, self->context), RES == 1);
    return 0;
}

static void wgl_destroyContext(struct wgl *self, void *dc) {
    debug_CHECK(self->wglMakeCurrent(dc, NULL), RES == 1);
    debug_CHECK(self->wglDeleteContext(self->context), RES == 1);
}

static void wgl_deinit(struct wgl *self) {
    debug_CHECK(FreeLibrary(self->dlHandle), RES != 0);
}

hc_UNUSED
static void *wgl_getProcAddress(struct wgl *self, const char *procName) {
    void *proc = self->wglGetProcAddress(procName);
    if (proc == NULL) proc = GetProcAddress(self->dlHandle, procName);
    return proc;
}

hc_UNUSED
static int32_t wgl_swapInterval(struct wgl *self, int32_t interval) {
    return self->wglSwapIntervalEXT(interval);
}

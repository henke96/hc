struct wgl {
    void *context;
    void *dc;
    void *dlHandle;
    void *(*wglGetProcAddress)(const char *name);
    void *(*wglCreateContext)(void *dc);
    int32_t (*wglMakeCurrent)(void *dc, void *context);
    int32_t (*wglDeleteContext)(void *context);
    void *(*wglCreateContextAttribsARB)(void *dc, void *shareContext, const int32_t *attributes);
    int32_t (*wglSwapIntervalEXT)(int32_t interval);
};

static int32_t wgl_init(struct wgl *self, void *dc) {
    self->dc = dc;

    self->dlHandle = LoadLibraryW(u"opengl32.dll");
    if (self->dlHandle == NULL) return -1;

    int32_t status = -2;
    if ((self->wglGetProcAddress = GetProcAddress(self->dlHandle, "wglGetProcAddress")) == NULL) goto cleanup_dlHandle;
    if ((self->wglCreateContext = GetProcAddress(self->dlHandle, "wglCreateContext")) == NULL) goto cleanup_dlHandle;
    if ((self->wglMakeCurrent = GetProcAddress(self->dlHandle, "wglMakeCurrent")) == NULL) goto cleanup_dlHandle;
    if ((self->wglDeleteContext = GetProcAddress(self->dlHandle, "wglDeleteContext")) == NULL) goto cleanup_dlHandle;

    // Create the dummy context.
    self->context = self->wglCreateContext(self->dc);
    if (self->context == NULL) {
        status = -3;
        goto cleanup_dlHandle;
    }

    if (!self->wglMakeCurrent(self->dc, self->context)) {
        status = -4;
        goto cleanup_context;
    }

    // Load extension functions.
    status = -5;
    if ((self->wglCreateContextAttribsARB = self->wglGetProcAddress("wglCreateContextAttribsARB")) == NULL) goto cleanup_context_current;
    if ((self->wglSwapIntervalEXT = self->wglGetProcAddress("wglSwapIntervalEXT")) == NULL) goto cleanup_context_current;

    return 0;

    cleanup_context_current:
    self->wglMakeCurrent(self->dc, NULL);
    cleanup_context:
    self->wglDeleteContext(self->context);
    cleanup_dlHandle:
    FreeLibrary(self->dlHandle);
    return status;
}

static int32_t wgl_updateContext(struct wgl *self, const int32_t *contextAttributes) {
    void *newContext = self->wglCreateContextAttribsARB(self->dc, NULL, contextAttributes);
    if (newContext == NULL) return -1;

    self->wglMakeCurrent(self->dc, NULL);
    self->wglDeleteContext(self->context);

    self->context = newContext;
    self->wglMakeCurrent(self->dc, self->context);
    return 0;
}

static void wgl_deinit(struct wgl *self) {
    self->wglMakeCurrent(self->dc, NULL);
    self->wglDeleteContext(self->context);
    FreeLibrary(self->dlHandle);
}

hc_UNUSED
static void *wgl_getProcAddress(struct wgl *self, const char *procName) {
    void *proc = self->wglGetProcAddress(procName);
    if (proc == NULL) proc = GetProcAddress(self->dlHandle, procName);
    return proc;
}

hc_UNUSED
static inline int32_t wgl_swapInterval(struct wgl *self, int32_t interval) {
    return self->wglSwapIntervalEXT(interval);
}

static inline int32_t wgl_swapBuffers(struct wgl *self) {
    return SwapBuffers(self->dc);
}

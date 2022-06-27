struct egl {
    void *display;
    void *config;
    void *context;
    void *surface;
    void *dlHandle;
    void *(*eglGetDisplay)(void *display);
    uint32_t (*eglInitialize)(void *display, int32_t *major, int32_t *minor);
    uint32_t (*eglChooseConfig)(void *display, const int32_t *attributes, void *configs, int32_t configsLength, int32_t *numConfigs);
    uint32_t (*eglBindAPI)(uint32_t api);
    void *(*eglCreateContext)(void *display, void *config, void *context, const int32_t *attributes);
    uint32_t (*eglGetConfigAttrib)(void *display, void *config, int32_t attribute, int32_t *value);
    void *(*eglCreateWindowSurface)(void *display, void *config, void *nativeWindow, const int32_t *attributes);
    uint32_t (*eglMakeCurrent)(void *display, void *drawSurface, void *readSurface, void *context);
    uint32_t (*eglSwapBuffers)(void *display, void *surface);
    uint32_t (*eglTerminate)(void *display);
    uint32_t (*eglDestroySurface)(void *display, void *surface);
    uint32_t (*eglDestroyContext)(void *display, void *context);
    void *(*eglGetProcAddress)(const char *procName);
    uint32_t (*eglSwapInterval)(void *display, int32_t interval);
};

static int32_t egl_init(struct egl *self) {
    self->context = egl_NO_CONTEXT;
    self->surface = egl_NO_SURFACE;

    self->dlHandle = dlopen("libEGL.so.1", RTLD_NOW);
    if (dlerror() != NULL) return -1;

    self->eglGetDisplay = dlsym(self->dlHandle, "eglGetDisplay");
    self->eglInitialize = dlsym(self->dlHandle, "eglInitialize");
    self->eglChooseConfig = dlsym(self->dlHandle, "eglChooseConfig");
    self->eglBindAPI = dlsym(self->dlHandle, "eglBindAPI");
    self->eglCreateContext = dlsym(self->dlHandle, "eglCreateContext");
    self->eglGetConfigAttrib = dlsym(self->dlHandle, "eglGetConfigAttrib");
    self->eglCreateWindowSurface = dlsym(self->dlHandle, "eglCreateWindowSurface");
    self->eglMakeCurrent = dlsym(self->dlHandle, "eglMakeCurrent");
    self->eglSwapBuffers = dlsym(self->dlHandle, "eglSwapBuffers");
    self->eglTerminate = dlsym(self->dlHandle, "eglTerminate");
    self->eglDestroySurface = dlsym(self->dlHandle, "eglDestroySurface");
    self->eglDestroyContext = dlsym(self->dlHandle, "eglDestroyContext");
    self->eglGetProcAddress = dlsym(self->dlHandle, "eglGetProcAddress");
    self->eglSwapInterval = dlsym(self->dlHandle, "eglSwapInterval");
    int32_t status;
    if (dlerror() != NULL) {
        status = -2;
        goto cleanup_dlHandle;
    }

    self->display = self->eglGetDisplay(egl_DEFAULT_DISPLAY); // TODO: What happens on XWayland? Could consider eglGetPlatformDisplay().
    if (self->display == egl_NO_DISPLAY) {
        status = -3;
        goto cleanup_dlHandle;
    }

    if (!self->eglInitialize(self->display, NULL, NULL)) {
        status = -4;
        goto cleanup_dlHandle;
    }
    return 0;

    cleanup_dlHandle:
    debug_CHECK(dlclose(self->dlHandle), RES == 0);
    return status;
}

// Returns visualId, or negative error code.
static int32_t egl_createContext(struct egl *self, uint32_t api, const int32_t *configAttributes, const int32_t *contextAttributes) {
    int32_t numConfigs;
    if (!self->eglChooseConfig(self->display, configAttributes, &self->config, 1, &numConfigs)) return -1;
    if (numConfigs != 1) return -2;

    if (!self->eglBindAPI(api)) return -3;

    int32_t visualId;
    if (!self->eglGetConfigAttrib(self->display, self->config, egl_NATIVE_VISUAL_ID, &visualId)) return -4;

    self->context = self->eglCreateContext(self->display, self->config, egl_NO_CONTEXT, contextAttributes);
    if (self->context == egl_NO_CONTEXT) return -5;
    return visualId;
}

// Must be after `egl_createContext`.
static int32_t egl_setupSurface(struct egl *self, uint32_t windowId) {
    self->surface = self->eglCreateWindowSurface(self->display, self->config, (void *)(uint64_t)windowId, NULL);
    if (self->surface == egl_NO_SURFACE) return -1;

    if (!self->eglMakeCurrent(self->display, self->surface, self->surface, self->context)) {
        debug_CHECK(self->eglDestroySurface(self->display, self->surface), RES == egl_TRUE);
        self->surface = egl_NO_SURFACE;
        return -2;
    }
    return 0;
}

// Returns boolean of success.
static inline uint32_t egl_swapBuffers(struct egl *self) {
    return self->eglSwapBuffers(self->display, self->surface);
}

static inline void *egl_getProcAddress(struct egl *self, const char *procName) {
    return self->eglGetProcAddress(procName);
}

static inline uint32_t egl_swapInterval(struct egl *self, int32_t interval) {
    return self->eglSwapInterval(self->display, interval);
}

static void egl_deinit(struct egl *self) {
    if (self->surface != egl_NO_SURFACE) {
        debug_CHECK(self->eglMakeCurrent(self->display, egl_NO_SURFACE, egl_NO_SURFACE, egl_NO_CONTEXT), RES == egl_TRUE);
        debug_CHECK(self->eglDestroySurface(self->display, self->surface), RES == egl_TRUE);
    }
    if (self->context != egl_NO_CONTEXT) debug_CHECK(self->eglDestroyContext(self->display, self->context), RES == egl_TRUE);

    debug_CHECK(self->eglTerminate(self->display), RES == egl_TRUE);
    debug_CHECK(dlclose(self->dlHandle), RES == 0);
}

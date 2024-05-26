struct egl {
    void *display;
    void *config;
    void *context;
    void *surface;
    void *dlHandle;
    void *(*eglGetDisplay)(void *display);
    uint32_t (*eglInitialize)(void *display, int32_t *major, int32_t *minor);
    int32_t (*eglGetError)(void);
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
    uint32_t (*eglQuerySurface)(void *display, void *surface, int32_t attribute, int32_t *value);
    void *(*eglGetPlatformDisplay)(uint32_t platform, void *display, const uint64_t *attributes);
};

// Returns visualId, or negative error code.
static int32_t egl_createContext(
    struct egl *self,
    int32_t eglPlatform, // -1 to use `eglGetDisplay()`.
    const uint64_t *platformAttrs, // Only used if `eglPlatform` is specified.
    void *nativeDisplay,
    uint32_t api,
    const int32_t *configAttributes,
    const int32_t *contextAttributes
) {
    if (eglPlatform < 0) {
        self->display = self->eglGetDisplay(nativeDisplay);
    } else if (self->eglGetPlatformDisplay != NULL) {
        self->display = self->eglGetPlatformDisplay((uint32_t)eglPlatform, nativeDisplay, platformAttrs);
    } else return -1;
    if (self->display == egl_NO_DISPLAY) return -2;

    if (!self->eglInitialize(self->display, NULL, NULL)) return -3;

    int32_t status;
    int32_t numConfigs;
    if (!self->eglChooseConfig(self->display, configAttributes, &self->config, 1, &numConfigs)) {
        status = -4;
        goto cleanup_display;
    }
    if (numConfigs != 1) {
        status = -5;
        goto cleanup_display;
    }

    if (!self->eglBindAPI(api)) {
        status = -6;
        goto cleanup_display;
    }

    int32_t visualId;
    if (!self->eglGetConfigAttrib(self->display, self->config, egl_NATIVE_VISUAL_ID, &visualId)) {
        status = -7;
        goto cleanup_display;
    }

    self->context = self->eglCreateContext(self->display, self->config, egl_NO_CONTEXT, contextAttributes);
    if (self->context == egl_NO_CONTEXT) {
        status = -8;
        goto cleanup_display;
    }
    return visualId;

    cleanup_display:
    debug_CHECK(self->eglTerminate(self->display), RES == egl_TRUE);
    return status;
}

static void egl_destroyContext(struct egl *self) {
    debug_CHECK(self->eglMakeCurrent(self->display, egl_NO_SURFACE, egl_NO_SURFACE, egl_NO_CONTEXT), RES == egl_TRUE);
    debug_CHECK(self->eglDestroyContext(self->display, self->context), RES == egl_TRUE);
    debug_CHECK(self->eglTerminate(self->display), RES == egl_TRUE);
}

static void egl_destroySurface(struct egl *self) {
    debug_CHECK(self->eglDestroySurface(self->display, self->surface), RES == egl_TRUE);
}

static int32_t egl_createSurface(struct egl *self, void *window) {
    self->surface = self->eglCreateWindowSurface(self->display, self->config, window, NULL);
    if (self->surface == egl_NO_SURFACE) return -1;

    if (!self->eglMakeCurrent(self->display, self->surface, self->surface, self->context)) {
        egl_destroySurface(self);
        return -2;
    }
    return 0;
}

// Returns boolean of success.
hc_UNUSED
static uint32_t egl_swapBuffers(struct egl *self) {
    return self->eglSwapBuffers(self->display, self->surface);
}

hc_UNUSED
static void *egl_getProcAddress(struct egl *self, const char *procName) {
    return self->eglGetProcAddress(procName);
}

hc_UNUSED
static uint32_t egl_swapInterval(struct egl *self, int32_t interval) {
    return self->eglSwapInterval(self->display, interval);
}

hc_UNUSED
static uint32_t egl_querySurface(struct egl *self, int32_t attribute, int32_t *value) {
    return self->eglQuerySurface(self->display, self->surface, attribute, value);
}

hc_UNUSED
static int32_t egl_getError(struct egl *self) {
    return self->eglGetError();
}

static int32_t egl_init(
    struct egl *self,
    const char *eglLibPath
) {
    self->dlHandle = dlopen(eglLibPath, RTLD_NOW);
    if (self->dlHandle == NULL) return -1;

    dlerror(); // Reset errors.
    self->eglGetDisplay = dlsym(self->dlHandle, "eglGetDisplay");
    self->eglInitialize = dlsym(self->dlHandle, "eglInitialize");
    self->eglGetError = dlsym(self->dlHandle, "eglGetError");
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
    self->eglQuerySurface = dlsym(self->dlHandle, "eglQuerySurface");

    if (dlerror() != NULL) {
        debug_CHECK(dlclose(self->dlHandle), RES == 0);
        return -1;
    }

    // This is optional, so don't check for error.
    self->eglGetPlatformDisplay = dlsym(self->dlHandle, "eglGetPlatformDisplay");
    dlerror(); // Reset errors.
    return 0;
}

static void egl_deinit(struct egl *self) {
    debug_CHECK(dlclose(self->dlHandle), RES == 0);
}

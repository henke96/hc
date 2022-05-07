static void *(*eglGetDisplay)(void *display);
static uint32_t (*eglInitialize)(void *display, int32_t *major, int32_t *minor);
static uint32_t (*eglChooseConfig)(void *display, const int32_t *attributes, void *configs, int32_t configsLength, int32_t *numConfigs);
static uint32_t (*eglBindAPI)(uint32_t api);
static void *(*eglCreateContext)(void *display, void *config, void *context, const int32_t *attributes);
static uint32_t (*eglGetConfigAttrib)(void *display, void *config, int32_t attribute, int32_t *value);
static void *(*eglCreateWindowSurface)(void *display, void *config, void *nativeWindow, const int32_t *attributes);
static uint32_t (*eglMakeCurrent)(void *display, void *drawSurface, void *readSurface, void *context);
static uint32_t (*eglSwapBuffers)(void *display, void *surface);

int32_t egl_init(void) {
    void *eglHandle = dlopen("libEGL.so.1", RTLD_NOW);
    if (dlerror() != NULL) return -1;
    eglGetDisplay = dlsym(eglHandle, "eglGetDisplay");
    if (dlerror() != NULL) return -2;
    eglInitialize = dlsym(eglHandle, "eglInitialize");
    if (dlerror() != NULL) return -3;
    eglChooseConfig = dlsym(eglHandle, "eglChooseConfig");
    if (dlerror() != NULL) return -4;
    eglBindAPI = dlsym(eglHandle, "eglBindAPI");
    if (dlerror() != NULL) return -5;
    eglCreateContext = dlsym(eglHandle, "eglCreateContext");
    if (dlerror() != NULL) return -6;
    eglGetConfigAttrib = dlsym(eglHandle, "eglGetConfigAttrib");
    if (dlerror() != NULL) return -7;
    eglCreateWindowSurface = dlsym(eglHandle, "eglCreateWindowSurface");
    if (dlerror() != NULL) return -8;
    eglMakeCurrent = dlsym(eglHandle, "eglMakeCurrent");
    if (dlerror() != NULL) return -9;
    eglSwapBuffers = dlsym(eglHandle, "eglSwapBuffers");
    if (dlerror() != NULL) return -10;
    return 0;
}

struct eglContext {
    void *display;
    void *config;
    void *context;
    void *surface;
};

// Returns visualId, or negative error code.
int32_t eglContext_init(struct eglContext *self) {
    self->display = eglGetDisplay(NULL);
    if (self->display == EGL_NO_DISPLAY) return -1;

    if (!eglInitialize(self->display, NULL, NULL)) return -2;

    const int32_t configAttributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_NONE
    };
    int32_t numConfigs;
    if (!eglChooseConfig(self->display, &configAttributes[0], &self->config, 1, &numConfigs)) return -3;

    if (!eglBindAPI(EGL_OPENGL_API)) return -4;

    const int32_t contextAttributes[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 2,
        EGL_NONE
    };
    self->context = eglCreateContext(self->display, self->config, EGL_NO_CONTEXT, &contextAttributes[0]);
    if (self->context == EGL_NO_CONTEXT) return -5;

    int32_t visualId;
    if (!eglGetConfigAttrib(self->display, self->config, EGL_NATIVE_VISUAL_ID, &visualId)) return -6;
    return visualId;
}

int32_t eglContext_setupSurface(struct eglContext *self, uint32_t windowId) {
    self->surface = eglCreateWindowSurface(self->display, self->config, (void *)(uint64_t)windowId, NULL);
    if (self->surface == EGL_NO_SURFACE) return -1;

    if (!eglMakeCurrent(self->display, self->surface, self->surface, self->context)) return -2;
    return 0;
}
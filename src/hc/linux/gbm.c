#define gbm_BO_USE_SCANOUT 0x1
#define gbm_BO_USE_CURSOR 0x2
#define gbm_BO_USE_RENDERING 0x4
#define gbm_BO_USE_WRITE 0x8
#define gbm_BO_USE_LINEAR 0x10
#define gbm_BO_USE_PROTECTED 0x20
#define gbm_BO_USE_FRONT_RENDERING 0x40

struct gbm {
    void *dlHandle;
    void (*gbmDeviceDestroy)(void *device);
    void *(*gbmSurfaceCreate)(void *device, uint32_t width, uint32_t height, uint32_t format, uint32_t flags);
    void (*gbmSurfaceDestroy)(void *surface);
    void *(*gbmSurfaceLockFrontBuffer)(void *surface);
    void (*gbmSurfaceReleaseBuffer)(void *surface, void *bo);
    void *(*gbmBoGetUserData)(void *bo);
    void (*gbmBoSetUserData)(void *bo, void *data, void (*destroy)(void *bo, void *data));
    uint32_t (*gbmBoGetStride)(void *bo);
    uint32_t (*gbmBoGetHandle)(void *bo);
    void *device;
    int32_t drmFd;
    int32_t __pad;
};

static int32_t gbm_init(struct gbm *self, int32_t drmFd) {
    self->dlHandle = dlopen("libgbm.so.1", RTLD_NOW);
    if (self->dlHandle == NULL) return -1;

    dlerror(); // Reset the error.
    void *(*gbmCreateDevice)(int32_t fd) = dlsym(self->dlHandle, "gbm_create_device");
    self->gbmDeviceDestroy = dlsym(self->dlHandle, "gbm_device_destroy");
    self->gbmSurfaceCreate = dlsym(self->dlHandle, "gbm_surface_create");
    self->gbmSurfaceDestroy = dlsym(self->dlHandle, "gbm_surface_destroy");
    self->gbmSurfaceLockFrontBuffer = dlsym(self->dlHandle, "gbm_surface_lock_front_buffer");
    self->gbmSurfaceReleaseBuffer = dlsym(self->dlHandle, "gbm_surface_release_buffer");
    self->gbmBoGetUserData = dlsym(self->dlHandle, "gbm_bo_get_user_data");
    self->gbmBoSetUserData = dlsym(self->dlHandle, "gbm_bo_set_user_data");
    self->gbmBoGetStride = dlsym(self->dlHandle, "gbm_bo_get_stride");
    self->gbmBoGetHandle = dlsym(self->dlHandle, "gbm_bo_get_handle");
    if (dlerror() != NULL) goto cleanup_dlHandle;

    self->device = gbmCreateDevice(drmFd);
    if (self->device == NULL) goto cleanup_dlHandle;
    return 0;

    cleanup_dlHandle:
    debug_CHECK(dlclose(self->dlHandle), RES == 0);
    return -1;
}

static void gbm_deinit(struct gbm *self) {
    self->gbmDeviceDestroy(self->device);
    debug_CHECK(dlclose(self->dlHandle), RES == 0);
}

hc_UNUSED
static inline void *gbm_surfaceCreate(
    struct gbm *self,
    uint32_t width,
    uint32_t height,
    uint32_t format,
    uint32_t flags
) {
    return self->gbmSurfaceCreate(self->device, width, height, format, flags);
}

hc_UNUSED
static inline void gbm_surfaceDestroy(struct gbm *self, void *surface) {
    self->gbmSurfaceDestroy(surface);
}

hc_UNUSED
static inline void *gbm_surfaceLockFrontBuffer(struct gbm *self, void *surface) {
    return self->gbmSurfaceLockFrontBuffer(surface);
}

hc_UNUSED
static inline void gbm_surfaceReleaseBuffer(struct gbm *self, void *surface, void *bo) {
    self->gbmSurfaceReleaseBuffer(surface, bo);
}

hc_UNUSED
static inline void *gbm_boGetUserData(struct gbm *self, void *bo) {
    return self->gbmBoGetUserData(bo);
}

hc_UNUSED
static inline void gbm_boSetUserData(
    struct gbm *self,
    void *bo,
    void *data,
    void (*destroy)(void *bo, void *data)
) {
    self->gbmBoSetUserData(bo, data, destroy);
}

hc_UNUSED
static inline uint32_t gbm_boGetStride(struct gbm *self, void *bo) {
    return self->gbmBoGetStride(bo);
}

hc_UNUSED
static inline uint32_t gbm_boGetHandle(struct gbm *self, void *bo) {
    return self->gbmBoGetHandle(bo);
}

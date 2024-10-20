#define drm_MAX_CONNECTORS 16
#define drm_MAX_MODES 64

struct drm {
    struct drm_mode_get_connector connector;
    struct drm_mode_modeinfo modeInfos[drm_MAX_MODES];
    int32_t cardFd;
    uint32_t crtcId;
};

static int32_t drm_init(struct drm *self, const char *driCardPath) {
    self->cardFd = openat(-1, driCardPath, O_RDWR | O_CLOEXEC, 0);
    if (self->cardFd < 0) return -1;

    // Get a list of connectors and one crtc for this card.
    uint32_t connectorIds[drm_MAX_CONNECTORS] = {0};

    struct drm_mode_card_res cardResources = {
        .connector_id_ptr = &connectorIds[0],
        .count_connectors = drm_MAX_CONNECTORS,
        .crtc_id_ptr = &self->crtcId,
        .count_crtcs = 1
    };
    int32_t status = ioctl(self->cardFd, DRM_IOCTL_MODE_GETRESOURCES, &cardResources);
    if (status < 0) {
        status = -2;
        goto cleanup_cardFd;
    }

    if (cardResources.count_crtcs < 1) {
        status = -3;
        goto cleanup_cardFd;
    }

    // Iterate over the connectors to find a suitable one.
    for (uint32_t i = 0; i < cardResources.count_connectors; ++i) {
        hc_MEMSET(&self->connector, 0, sizeof(self->connector));
        self->connector.connector_id = connectorIds[i];
        self->connector.modes_ptr = &self->modeInfos[0];
        self->connector.count_modes = drm_MAX_MODES;

        status = ioctl(self->cardFd, DRM_IOCTL_MODE_GETCONNECTOR, &self->connector);
        if (status < 0) {
            status = -4;
            goto cleanup_cardFd;
        }

        if (
            self->connector.connection == DRM_CONNECTOR_STATUS_CONNECTED &&
            self->connector.count_modes > 0
        ) goto foundConnector;
    }
    // Did not find suitable connector.
    status = -5;
    goto cleanup_cardFd;

    foundConnector:
    // Kernel is silly and doesn't fill out any modes if it can't fit all of them..
    if (self->connector.count_modes > drm_MAX_MODES) {
        status = -6;
        goto cleanup_cardFd;
    }
    return 0;

    cleanup_cardFd:
    debug_CHECK(close(self->cardFd), RES == 0);
    return status;
}

hc_UNUSED
static int32_t drm_createDumbBuffer(struct drm *self, struct drm_mode_create_dumb *dumbBuffer) {
    return ioctl(self->cardFd, DRM_IOCTL_MODE_CREATE_DUMB, dumbBuffer);
}

hc_UNUSED
static void drm_destroyDumbBuffer(struct drm *self, uint32_t handle) {
    struct drm_mode_destroy_dumb destroy = { .handle = handle };
    debug_CHECK(ioctl(self->cardFd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy), RES == 0);
}

hc_UNUSED
static void *drm_mmapDumbBuffer(struct drm *self, uint32_t handle, int64_t length) {
    struct drm_mode_map_dumb mapDumpBuffer = { .handle = handle };
    int32_t status = ioctl(self->cardFd, DRM_IOCTL_MODE_MAP_DUMB, &mapDumpBuffer);
    if (status < 0) return (void *)(ssize_t)-1;

    return mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, self->cardFd, (int64_t)mapDumpBuffer.offset);
}

hc_UNUSED
static int32_t drm_createFrameBuffer(struct drm *self, struct drm_mode_fb_cmd *frameBuffer) {
    return ioctl(self->cardFd, DRM_IOCTL_MODE_ADDFB, frameBuffer);
}

hc_UNUSED
static void drm_destroyFrameBuffer(struct drm *self, uint32_t fbId) {
    debug_CHECK(ioctl(self->cardFd, DRM_IOCTL_MODE_RMFB, &fbId), RES == 0);
}


hc_UNUSED
static void drm_markFrameBufferDirty(struct drm *self, uint32_t fbId) {
    struct drm_mode_fb_dirty_cmd fbDirty = { .fb_id = fbId };
    debug_CHECK(ioctl(self->cardFd, DRM_IOCTL_MODE_DIRTYFB, &fbDirty), RES == 0 || ix_ERRNO(RES) == ENOSYS);
}

hc_UNUSED
static int32_t drm_setCrtc(struct drm *self, int32_t modeIndex, uint32_t fbId) {
    struct drm_mode_crtc setCrtc = {
        .set_connectors_ptr = &self->connector.connector_id,
        .count_connectors = 1,
        .crtc_id = self->crtcId,
        .fb_id = fbId,
        .mode_valid = 1,
        .mode = self->modeInfos[modeIndex]
    };
    return ioctl(self->cardFd, DRM_IOCTL_MODE_SETCRTC, &setCrtc);
}

hc_UNUSED
static int32_t drm_pageFlip(struct drm *self, uint32_t fbId, uint32_t flags) {
    struct drm_mode_crtc_page_flip pageFlip = {
        .crtc_id = self->crtcId,
        .fb_id = fbId,
        .flags = flags
    };
    return ioctl(self->cardFd, DRM_IOCTL_MODE_PAGE_FLIP, &pageFlip);
}

hc_UNUSED
static int32_t drm_awaitPageFlipEvent(struct drm *self) {
    struct drm_event_vblank event;
    event.base.type = 0; // Make static analysis happy.
    for (;;) {
        int64_t numRead = read(self->cardFd, &event, sizeof(event));
        if (numRead <= 0) return -1;
        if (event.base.type == DRM_EVENT_FLIP_COMPLETE) return 0;
    }
}

hc_UNUSED
static int32_t drm_bestModeIndex(struct drm *self) {
    int32_t bestModeIndex = 0;
    uint16_t bestModeWidth = 0;
    uint16_t bestModeHeight = 0;
    int32_t bestModeHz = 0;
    for (int32_t i = 0; i < self->connector.count_modes; ++i) {
        if (self->modeInfos[i].hdisplay < bestModeWidth) continue;
        if (self->modeInfos[i].hdisplay == bestModeWidth) {
            if (self->modeInfos[i].vdisplay < bestModeHeight) continue;
            if (self->modeInfos[i].vdisplay == bestModeHeight) {
                if (self->modeInfos[i].vrefresh <= bestModeHz) continue;
            }
        }
        bestModeIndex = i;
        bestModeWidth = self->modeInfos[i].hdisplay;
        bestModeHeight = self->modeInfos[i].vdisplay;
        bestModeHz = self->modeInfos[i].vrefresh;
    }
    return bestModeIndex;
}

static void drm_deinit(struct drm *self) {
    debug_CHECK(close(self->cardFd), RES == 0);
}

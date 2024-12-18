static int32_t window_gbm_init(void **eglWindow) {
    int32_t status = drm_init(&window.gbm.drm, "/dev/dri/card0");
    if (status < 0) {
        debug_printNum("Failed to initialise DRM/KMS", status);
        return -1;
    }
    window.gbm.drmModeIndex = drm_bestModeIndex(&window.gbm.drm);
    struct drm_mode_modeinfo *bestMode = &window.gbm.drm.modeInfos[window.gbm.drmModeIndex];

    status = gbm_init(&window.gbm.gbm, window.gbm.drm.cardFd);
    if (status < 0) goto cleanup_drm;

    status = egl_createContext(
        &window.egl,
        egl_PLATFORM_GBM_KHR,
        NULL,
        window.gbm.gbm.device,
        egl_OPENGL_ES_API,
        &window_configAttributes[0],
        &window_contextAttributes[0]
    );
    if (status < 0) {
        debug_printNum("Failed to initialise EGL context", status);
        goto cleanup_gbm;
    }
    window.gbm.gbmFormat = (uint32_t)status;

    window.gbm.gbmSurface = gbm_surfaceCreate(
        &window.gbm.gbm,
        bestMode->hdisplay,
        bestMode->vdisplay,
        window.gbm.gbmFormat,
        gbm_BO_USE_SCANOUT | gbm_BO_USE_RENDERING
    );
    if (window.gbm.gbmSurface == NULL) goto cleanup_eglContext;

    *eglWindow = window.gbm.gbmSurface;
    window.width = bestMode->hdisplay;
    window.height = bestMode->vdisplay;
    return 0;

    cleanup_eglContext:
    egl_destroyContext(&window.egl);
    cleanup_gbm:
    gbm_deinit(&window.gbm.gbm);
    cleanup_drm:
    drm_deinit(&window.gbm.drm);
    return -1;
}

static void window_gbm_destroyFb(hc_UNUSED void *bo, void *data) {
    drm_destroyFrameBuffer(&window.gbm.drm, (uint32_t)(size_t)data);
}

static int64_t window_gbm_getFbId(void *bo) {
    void *data = gbm_boGetUserData(&window.gbm.gbm, bo);
    if (data != NULL) return (int64_t)data;

    struct drm_mode_fb_cmd frameBuffer = {
        .width = window.width,
        .height = window.height,
        .pitch = gbm_boGetStride(&window.gbm.gbm, bo),
        .bpp = 32,
        .depth = 24,
        .handle = gbm_boGetHandle(&window.gbm.gbm, bo)
    };

    int32_t status = drm_createFrameBuffer(&window.gbm.drm, &frameBuffer);
    if (status < 0) return -1;
    gbm_boSetUserData(
        &window.gbm.gbm,
        bo,
        (void *)(size_t)frameBuffer.fb_id,
        window_gbm_destroyFb
    );
    return frameBuffer.fb_id;
}

static int32_t window_gbm_run(void) {
    // Finish DRM/GBM setup.
    if (egl_swapBuffers(&window.egl) != 1) return -1; // Sets front buffer.
    void *bo = gbm_surfaceLockFrontBuffer(&window.gbm.gbm, window.gbm.gbmSurface);
    if (bo == NULL) return -2;

    int64_t fbId = window_gbm_getFbId(bo);
    if (fbId < 0) return -3;

    int32_t status = drm_setCrtc(&window.gbm.drm, window.gbm.drmModeIndex, (uint32_t)fbId);
    if (status < 0) {
        debug_printNum("Failed to set CRTC", status);
        return -4;
    }

    // Main loop.
    for (;;) {
        struct timespec drawTimespec;
        debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &drawTimespec), RES == 0);
        uint64_t drawTimestamp = (uint64_t)drawTimespec.tv_sec * 1000000000 + (uint64_t)drawTimespec.tv_nsec;
        game_draw(drawTimestamp, false);
        debug_CHECK(egl_swapBuffers(&window.egl), RES == 1);

        void *nextBo = gbm_surfaceLockFrontBuffer(&window.gbm.gbm, window.gbm.gbmSurface);
        if (nextBo == NULL) return -6;
        fbId = window_gbm_getFbId(nextBo);
        if (fbId < 0) return -7;

        status = drm_pageFlip(&window.gbm.drm, (uint32_t)fbId, DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_PAGE_FLIP_ASYNC);
        if (status < 0) return -8;

        status = drm_awaitPageFlipEvent(&window.gbm.drm);
        if (status < 0) return -9;

        gbm_surfaceReleaseBuffer(&window.gbm.gbm, window.gbm.gbmSurface, bo);
        bo = nextBo;
    }
}

static void window_gbm_deinit(void) {
    gbm_surfaceDestroy(&window.gbm.gbm, window.gbm.gbmSurface);
    egl_destroyContext(&window.egl);
    gbm_deinit(&window.gbm.gbm);
    drm_deinit(&window.gbm.drm);
}

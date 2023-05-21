struct graphics {
    int64_t frameBufferSize;
    uint32_t *frameBuffer;
    struct drmKms drmKms;
    struct drm_mode_fb_cmd frameBufferInfo;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
};

static int32_t graphics_init(struct graphics *self) {
    self->red = 0;
    self->green = 0;
    self->blue = 0;

    int32_t status = drmKms_init(&self->drmKms, "/dev/dri/card0");
    if (status < 0) {
        debug_printNum("Failed to initialise DRM/KMS (", status, ")\n");
        return -1;
    }

    int32_t modeIndex = drmKms_bestModeIndex(&self->drmKms);
    struct iovec_const print[] = {
        { hc_STR_COMMA_LEN("Selected mode \"") },
        { self->drmKms.modeInfos[modeIndex].name, DRM_DISPLAY_MODE_LEN }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
    debug_printNum("\" at ", self->drmKms.modeInfos[modeIndex].vrefresh, " Hz.\n");

    // Setup frame buffer using the selected mode.
    struct drm_mode_create_dumb dumbBuffer = {
        .width = self->drmKms.modeInfos[modeIndex].hdisplay,
        .height = self->drmKms.modeInfos[modeIndex].vdisplay,
        .bpp = 32
    };
    status = drmKms_createDumbBuffer(&self->drmKms, &dumbBuffer);
    if (status < 0) {
        debug_printNum("Failed to create dumb buffer (", status, "\n");
        goto cleanup_drmKms;
    }
    self->frameBufferSize = (int64_t)dumbBuffer.size;

    hc_MEMSET(&self->frameBufferInfo, 0, sizeof(self->frameBufferInfo));
    self->frameBufferInfo.width = dumbBuffer.width;
    self->frameBufferInfo.height = dumbBuffer.height;
    self->frameBufferInfo.pitch = dumbBuffer.pitch;
    self->frameBufferInfo.bpp = dumbBuffer.bpp;
    self->frameBufferInfo.depth = 24;
    self->frameBufferInfo.handle = dumbBuffer.handle;
    status = drmKms_createFrameBuffer(&self->drmKms, &self->frameBufferInfo);
    if (status < 0) {
        debug_printNum("Failed to create frame buffer (", status, "\n");
        goto cleanup_dumbBuffer;
    }

    status = drmKms_setCrtc(&self->drmKms, modeIndex, self->frameBufferInfo.fb_id);
    if (status < 0) {
        debug_printNum("Failed to set CRTC (", status, "\n");
        goto cleanup_frameBuffer;
    }

    // Map the frame buffer.
    self->frameBuffer = drmKms_mmapDumbBuffer(&self->drmKms, self->frameBufferInfo.handle, self->frameBufferSize);
    if ((ssize_t)self->frameBuffer < 0) {
        debug_printNum("Failed to map frame buffer (", status, "\n");
        goto cleanup_frameBuffer;
    }
    return 0;

    cleanup_frameBuffer:
    drmKms_destroyFrameBuffer(&self->drmKms, self->frameBufferInfo.fb_id);
    cleanup_dumbBuffer:
    drmKms_destroyDumbBuffer(&self->drmKms, dumbBuffer.handle);
    cleanup_drmKms:
    drmKms_deinit(&self->drmKms);
    return -1;
}

static void graphics_deinit(struct graphics *self) {
    debug_CHECK(sys_munmap(self->frameBuffer, self->frameBufferSize), RES == 0);
    drmKms_destroyFrameBuffer(&self->drmKms, self->frameBufferInfo.fb_id);
    drmKms_destroyDumbBuffer(&self->drmKms, self->frameBufferInfo.handle);
    drmKms_deinit(&self->drmKms);
}

static void graphics_draw(struct graphics *self) {
    uint32_t red = self->red;
    uint32_t green = self->green;
    uint32_t blue = self->blue;

    // Do drawing.
    uint32_t colour = (red << 16) | (green << 8) | blue;
    int32_t numPixels = (int32_t)(self->frameBufferSize >> 2);
    for (int32_t i = 0; i < numPixels; ++i) self->frameBuffer[i] = colour;
    drmKms_markFrameBufferDirty(&self->drmKms, self->frameBufferInfo.fb_id);

    // Continuous iteration of colours.
    if (red == 0 && green == 0 && blue != 255) ++blue;
    else if (red == 0 && green != 255 && blue == 255) ++green;
    else if (red == 0 && green == 255 && blue != 0) --blue;
    else if (red != 255 && green == 255 && blue == 0) ++red;
    else if (red == 255 && green == 255 && blue != 255) ++blue;
    else if (red == 255 && green != 0 && blue == 255) --green;
    else if (red == 255 && green == 0 && blue != 0) --blue;
    else --red;

    self->red = red;
    self->green = green;
    self->blue = blue;
}

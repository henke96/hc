enum window_state {
    window_INIT,
    window_RUNNING
};

struct window {
    struct x11Client x11Client;
    struct egl egl;
    enum window_state state;
    uint32_t windowId;
};

static int32_t window_init(struct window *self, char **envp) {
    self->state = window_INIT;

    // Initialise EGL.
    int32_t status = egl_init(&self->egl);
    if (status < 0) {
        printf("Failed to initalise EGL (%d)\n", status);
        return -1;
    }
    const int32_t configAttributes[] = {
        egl_RED_SIZE, 8,
        egl_GREEN_SIZE, 8,
        egl_BLUE_SIZE, 8,
        egl_NONE
    };
    const int32_t contextAttributes[] = {
        egl_CONTEXT_MAJOR_VERSION, 3,
        egl_CONTEXT_MINOR_VERSION, 0,
        egl_NONE
    };
    status = egl_createContext(&self->egl, egl_OPENGL_ES_API, &configAttributes[0], &contextAttributes[0]);
    if (status < 0) {
        printf("Failed to create EGL context (%d)\n", status);
        goto cleanup_egl;
    }
    uint32_t eglVisualId = (uint32_t)status;

    // Initialise x11.
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    static const char address[] = "/tmp/.X11-unix/X0";
    hc_MEMCPY(&serverAddr.sun_path[0], &address[0], sizeof(address));
    char *xAuthorityFile = util_getEnv(envp, "XAUTHORITY");
    struct xauth xauth;
    if (xAuthorityFile != NULL && xauth_init(&xauth, xAuthorityFile) == 0) {
        struct xauth_entry entry = {0}; // Zeroed in case `xauth_nextEntry` fails.
        xauth_nextEntry(&xauth, &entry);
        status = x11Client_init(&self->x11Client, &serverAddr, sizeof(serverAddr), &entry);
        xauth_deinit(&xauth);
    } else {
        status = x11Client_init(&self->x11Client, &serverAddr, sizeof(serverAddr), &(struct xauth_entry) {0});
    }
    if (status < 0) {
        printf("Failed to initialise x11Client (%d)\n", status);
        goto cleanup_egl;
    }

    // Send requests to create and map X11 window.
    self->windowId = x11Client_nextId(&self->x11Client);
    uint64_t rootsOffset = (
        util_ALIGN_FORWARD(self->x11Client.setupResponse->vendorLength, 4) +
        sizeof(struct x11_format) * self->x11Client.setupResponse->numPixmapFormats
    );
    struct x11_screen *screen = (void *)&self->x11Client.setupResponse->data[rootsOffset]; // Use first screen.
    uint32_t parentId = screen->windowId;

    struct requests {
        struct x11_createWindow createWindow;
        uint32_t createWindowValues[1];
        struct x11_mapWindow mapWindow;
    };

    struct requests windowRequests = {
        .createWindow = {
            .opcode = x11_createWindow_OPCODE,
            .depth = 0,
            .length = (sizeof(windowRequests.createWindow) + sizeof(windowRequests.createWindowValues)) / 4,
            .windowId = self->windowId,
            .parentId = parentId,
            .width = 200,
            .height = 200,
            .borderWidth = 1,
            .class = x11_INPUT_OUTPUT,
            .visualId = eglVisualId,
            .valueMask = x11_createWindow_EVENT_MASK
        },
        .createWindowValues = {
            x11_EVENT_STRUCTURE_NOTIFY_BIT | x11_EVENT_EXPOSURE_BIT
        },
        .mapWindow = {
            .opcode = x11_mapWindow_OPCODE,
            .length = sizeof(windowRequests.mapWindow) / 4,
            .windowId = windowRequests.createWindow.windowId
        }
    };

    if (x11Client_sendRequests(&self->x11Client, &windowRequests, sizeof(windowRequests), 2) < 0) {
        printf("Failed to send x11 window requests\n");
        goto cleanup_x11Client;
    }
    return 0;

    cleanup_x11Client:
    x11Client_deinit(&self->x11Client);
    cleanup_egl:
    egl_deinit(&self->egl);
    return -1;
}

static int32_t window_run(struct window *self) {
    for (;;) {
        int32_t msgLength = x11Client_nextMessage(&self->x11Client);
        if (msgLength == 0) {
            int32_t numRead = x11Client_receive(&self->x11Client);
            if (numRead == 0) return 0;
            if (numRead < 0) return -1;
            continue;
        }
        uint8_t msgType = self->x11Client.receiveBuffer[0];
        if (msgType == x11_TYPE_ERROR) {
            uint8_t errorCode = self->x11Client.receiveBuffer[1];
            uint16_t sequenceNumber = *(uint16_t *)&self->x11Client.receiveBuffer[2];
            printf("X11 request failed (seq=%d, code=%d)\n", (int32_t)sequenceNumber, (int32_t)errorCode);
            return -2; // For now we always exit on X11 errors.
        }

        switch (self->state) {
            case window_INIT: {
                // In this state we are waiting for a MapNotify event, everything else is ignored.
                if (msgType != x11_mapNotify_TYPE) {
                    printf("INIT: Ignored message type: %d\n", msgType);
                    break;
                }
                int32_t status = egl_setupSurface(&self->egl, (uint32_t)self->windowId);
                if (status < 0) {
                    printf("Failed to setup EGL surface (%d)\n", status);
                    return -3;
                }
                status = gl_init(&self->egl);
                if (status < 0) {
                    printf("Failed to initialise GL (%d)\n", status);
                    return -4;
                }
                // Try to disable vsync.
                egl_swapInterval(&self->egl, 0);

                status = game_init();
                if (status < 0) {
                    printf("Failed to initialise game (%d)\n", status);
                    return -5;
                }
                self->state = window_RUNNING;
                break;
            }
            case window_RUNNING: {
                printf("RUNNING: Got message type: %d\n", msgType);
                if (msgType == x11_configureNotify_TYPE) {
                    struct x11_configureNotify *configureNotify = (void *)&self->x11Client.receiveBuffer[0];
                    gl_viewport(0, 0, configureNotify->width, configureNotify->height);
                } else if (
                    msgType == x11_expose_TYPE &&
                    ((struct x11_expose *)&self->x11Client.receiveBuffer[0])->count == 0
                ) {
                    if (game_draw() < 0) return -6;
                    if (!egl_swapBuffers(&self->egl)) return -7;
                }
                break;
            }
            default: hc_UNREACHABLE;
        }
        x11Client_ackMessage(&self->x11Client, msgLength);
    }
}

static void window_deinit(struct window *self) {
    if (self->state == window_RUNNING) game_deinit();
    x11Client_deinit(&self->x11Client);
    egl_deinit(&self->egl);
}
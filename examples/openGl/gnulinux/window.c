#define window_DEFAULT_WIDTH 640
#define window_DEFAULT_HEIGHT 480

struct window {
    struct x11Client x11Client;
    struct egl egl;
    int32_t epollFd;
    uint32_t windowId;
    uint32_t rootWindowId;
    uint32_t wmProtocolsAtom;
    uint32_t wmDeleteWindowAtom;
    uint8_t xinputMajorOpcode;
    uint8_t xfixesMajorOpcode;
    char __pad[2];
};

static struct window window;

static int32_t window_x11Setup(uint32_t visualId) {
    window.windowId = x11Client_nextId(&window.x11Client);
    uint64_t rootsOffset = (
        util_ALIGN_FORWARD(window.x11Client.setupResponse->vendorLength, 4) +
        sizeof(struct x11_format) * window.x11Client.setupResponse->numPixmapFormats
    );
    struct x11_screen *screen = (void *)&window.x11Client.setupResponse->data[rootsOffset]; // Use first screen.
    window.rootWindowId = screen->windowId;

    struct requests {
        struct x11_createWindow createWindow;
        uint32_t createWindowValues[1];
        struct x11_mapWindow mapWindow;
        struct x11_queryExtension queryXfixes;
        char queryXfixesName[sizeof(x11_XFIXES_NAME) - 1];
        uint8_t queryXfixesPad[util_PAD_BYTES(sizeof(x11_XFIXES_NAME) - 1, 4)];
        struct x11_queryExtension queryXinput;
        char queryXinputName[sizeof(x11_XINPUT_NAME) - 1];
        uint8_t queryXinputPad[util_PAD_BYTES(sizeof(x11_XINPUT_NAME) - 1, 4)];
        struct x11_internAtom wmProtocolsAtom;
        char wmProtocolsAtomName[sizeof("WM_PROTOCOLS") - 1];
        uint8_t wmProtocolsAtomPad[util_PAD_BYTES(sizeof("WM_PROTOCOLS") - 1, 4)];
        struct x11_internAtom wmDeleteWindowAtom;
        char wmDeleteWindowAtomName[sizeof("WM_DELETE_WINDOW") - 1];
        uint8_t wmDeleteWindowAtomPad[util_PAD_BYTES(sizeof("WM_DELETE_WINDOW") - 1, 4)];
    };

    struct requests windowRequests = {
        .createWindow = {
            .opcode = x11_createWindow_OPCODE,
            .depth = 0,
            .length = (sizeof(windowRequests.createWindow) + sizeof(windowRequests.createWindowValues)) / 4,
            .windowId = window.windowId,
            .parentId = window.rootWindowId,
            .width = window_DEFAULT_WIDTH,
            .height = window_DEFAULT_HEIGHT,
            .borderWidth = 1,
            .class = x11_INPUT_OUTPUT,
            .visualId = visualId,
            .valueMask = x11_createWindow_EVENT_MASK
        },
        .createWindowValues = {
            x11_EVENT_STRUCTURE_NOTIFY_BIT
        },
        .mapWindow = {
            .opcode = x11_mapWindow_OPCODE,
            .length = sizeof(windowRequests.mapWindow) / 4,
            .windowId = windowRequests.createWindow.windowId
        },
        .queryXfixes = {
            .opcode = x11_queryExtension_OPCODE,
            .length = (sizeof(windowRequests.queryXfixes) + sizeof(windowRequests.queryXfixesName) + sizeof(windowRequests.queryXfixesPad)) / 4,
            .nameLength = sizeof(windowRequests.queryXfixesName),
        },
        .queryXfixesName = x11_XFIXES_NAME,
        .queryXinput = {
            .opcode = x11_queryExtension_OPCODE,
            .length = (sizeof(windowRequests.queryXinput) + sizeof(windowRequests.queryXinputName) + sizeof(windowRequests.queryXinputPad)) / 4,
            .nameLength = sizeof(windowRequests.queryXinputName),
        },
        .queryXinputName = x11_XINPUT_NAME,
        .wmProtocolsAtom = {
            .opcode = x11_internAtom_OPCODE,
            .onlyIfExists = 0,
            .length = (sizeof(windowRequests.wmProtocolsAtom) + sizeof(windowRequests.wmProtocolsAtomName) + sizeof(windowRequests.wmProtocolsAtomPad)) / 4,
            .nameLength = sizeof(windowRequests.wmProtocolsAtomName)
        },
        .wmProtocolsAtomName = "WM_PROTOCOLS",
        .wmDeleteWindowAtom = {
            .opcode = x11_internAtom_OPCODE,
            .onlyIfExists = 0,
            .length = (sizeof(windowRequests.wmDeleteWindowAtom) + sizeof(windowRequests.wmDeleteWindowAtomName) + sizeof(windowRequests.wmDeleteWindowAtomPad)) / 4,
            .nameLength = sizeof(windowRequests.wmDeleteWindowAtomName)
        },
        .wmDeleteWindowAtomName = "WM_DELETE_WINDOW"
    };
    int32_t status = x11Client_sendRequests(&window.x11Client, &windowRequests, sizeof(windowRequests), 6);
    if (status < 0) return -1;

    // Wait for all replies.
    int32_t nextSequenceNumber = 3;
    for (;;) {
        int32_t msgLength = x11Client_nextMessage(&window.x11Client);
        if (msgLength == 0) {
            int32_t numRead = x11Client_receive(&window.x11Client);
            if (numRead <= 0) return -2;
            continue;
        }
        struct x11_genericResponse *generic = (void *)&window.x11Client.receiveBuffer[0];
        if (generic->type == x11_TYPE_ERROR) {
            printf("X11 request failed (seq=%d, code=%d)\n", (int32_t)generic->sequenceNumber, (int32_t)generic->extra);
            return -3;
        }

        if (generic->type == x11_TYPE_REPLY) {
            if (generic->sequenceNumber != nextSequenceNumber) return -4;

            switch (nextSequenceNumber) {
                case 3: {
                    struct x11_queryExtensionResponse *response = (void *)generic;
                    if (!response->present) return -5;
                    window.xfixesMajorOpcode = response->majorOpcode;
                    break;
                }
                case 4: {
                    struct x11_queryExtensionResponse *response = (void *)generic;
                    if (!response->present) return -6;
                    window.xinputMajorOpcode = response->majorOpcode;
                    break;
                }
                case 5: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.wmProtocolsAtom = response->atom;
                    break;
                }
                case 6: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.wmDeleteWindowAtom = response->atom;
                    x11Client_ackMessage(&window.x11Client, msgLength);
                    return 0; // Done.
                }
                default: hc_UNREACHABLE;
            }
            ++nextSequenceNumber;
        }
        x11Client_ackMessage(&window.x11Client, msgLength);
    }
}

static int32_t window_init(char **envp) {
    // Initialise EGL.
    int32_t status = egl_init(&window.egl);
    if (status < 0) {
        printf("Failed to initalise EGL (%d)\n", status);
        return -1;
    }
    const int32_t configAttributes[] = {
        egl_RED_SIZE, 8,
        egl_GREEN_SIZE, 8,
        egl_BLUE_SIZE, 8,
        egl_DEPTH_SIZE, 24,
        egl_STENCIL_SIZE, 8,
        egl_NONE
    };
    const int32_t contextAttributes[] = {
        egl_CONTEXT_MAJOR_VERSION, 3,
        egl_CONTEXT_MINOR_VERSION, 0,
        egl_NONE
    };
    status = egl_createContext(&window.egl, egl_OPENGL_ES_API, &configAttributes[0], &contextAttributes[0]);
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
        status = x11Client_init(&window.x11Client, &serverAddr, sizeof(serverAddr), &entry);
        xauth_deinit(&xauth);
    } else {
        status = x11Client_init(&window.x11Client, &serverAddr, sizeof(serverAddr), &(struct xauth_entry) {0});
    }
    if (status < 0) {
        printf("Failed to initialise x11Client (%d)\n", status);
        goto cleanup_egl;
    }

    status = window_x11Setup(eglVisualId);
    if (status < 0) {
        printf("X11 setup failed (%d)\n", status);
        goto cleanup_x11Client;
    }

    // Setup EGL surface.
    status = egl_setupSurface(&window.egl, (uint32_t)window.windowId);
    if (status < 0) {
        printf("Failed to setup EGL surface (%d)\n", status);
        goto cleanup_x11Client;
    }
    debug_CHECK(egl_swapInterval(&window.egl, 0), == egl_TRUE);

    // Load OpenGL functions.
    status = gl_init(&window.egl);
    if (status < 0) {
        printf("Failed to initialise GL (%d)\n", status);
        goto cleanup_x11Client;
    }

    // Initialise game.
    status = game_init(
        window_DEFAULT_WIDTH,
        window_DEFAULT_HEIGHT
    );
    if (status < 0) {
        printf("Failed to initialise game (%d)\n", status);
        goto cleanup_x11Client;
    }

    // Setup epoll.
    window.epollFd = sys_epoll_create1(0);
    if (window.epollFd < 0) {
        status = -6;
        goto cleanup_x11Client;
    }
    struct epoll_event x11SocketEvent = {
        .events = EPOLLIN,
        .data.ptr = &window.x11Client.socketFd
    };
    if (sys_epoll_ctl(window.epollFd, EPOLL_CTL_ADD, window.x11Client.socketFd, &x11SocketEvent) < 0) {
        status = -7;
        goto cleanup_epollFd;
    }

    return 0;

    cleanup_epollFd:
    debug_CHECK(sys_close(window.epollFd), == 0);
    cleanup_x11Client:
    x11Client_deinit(&window.x11Client);
    cleanup_egl:
    egl_deinit(&window.egl);
    return status;
}

static int32_t window_run(void) {
    uint64_t frameCounter = 0;
    struct timespec prev;
    debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &prev), == 0);

    // Grab pointer.
    struct requests {
        struct x11_changeProperty changeProperty;
        uint32_t changePropertyData;
        struct x11_xfixesQueryVersion queryXfixesVersion; // Need to do this once to tell server what version we expect.
        struct x11_grabPointer grabPointer;
        struct x11_xfixesHideCursor hideCursor;
        struct x11_xinputSelectEvents xinputSelectEvents;
        struct x11_xinputEventMask xinputSelectEventsMask;
    };
    struct requests grabPointerRequests = {
        .changeProperty = {
            .opcode = x11_changeProperty_OPCODE,
            .mode = x11_changeProperty_REPLACE,
            .length = (sizeof(grabPointerRequests.changeProperty) + sizeof(grabPointerRequests.changePropertyData)) / 4,
            .window = window.windowId,
            .property = window.wmProtocolsAtom,
            .type = x11_ATOM_ATOM,
            .format = 32,
            .dataLength = 1
        },
        .changePropertyData = window.wmDeleteWindowAtom,
        .queryXfixesVersion = {
            .majorOpcode = window.xfixesMajorOpcode,
            .opcode = x11_xfixesQueryVersion_OPCODE,
            .length = sizeof(grabPointerRequests.queryXfixesVersion) / 4,
            .majorVersion = 4,
            .minorVersion = 0
        },
        .grabPointer = {
            .opcode = x11_grabPointer_OPCODE,
            .ownerEvents = x11_TRUE,
            .length = sizeof(grabPointerRequests.grabPointer) / 4,
            .grabWindowId = window.windowId,
            .eventMask = x11_EVENT_BUTTON_PRESS_BIT | x11_EVENT_BUTTON_RELEASE_BIT,
            .pointerMode = x11_grabPointer_ASYNCHRONOUS,
            .keyboardMode = x11_grabPointer_ASYNCHRONOUS,
            .confineToWindowId = window.windowId,
            .cursor = 0,
            .time = 0 // CurrentTime
        },
        .hideCursor = {
            .majorOpcode = window.xfixesMajorOpcode,
            .opcode = x11_xfixesHideCursor_OPCODE,
            .length = sizeof(grabPointerRequests.hideCursor) / 4,
            .windowId = window.windowId
        },
        .xinputSelectEvents = {
            .majorOpcode = window.xinputMajorOpcode,
            .opcode = x11_xinputSelectEvents_OPCODE,
            .length = (sizeof(grabPointerRequests.xinputSelectEvents) + sizeof(grabPointerRequests.xinputSelectEventsMask)) / 4,
            .windowId = window.rootWindowId,
            .numMasks = 1
        },
        .xinputSelectEventsMask = {
            .deviceId = x11_XINPUT_ALL_MASTER_DEVICES,
            .maskLength = 1,
            .mask = (1 << x11_XINPUT_RAW_MOTION)
        }
    };
    if (x11Client_sendRequests(&window.x11Client, &grabPointerRequests, sizeof(grabPointerRequests), 5) < 0) return -1;

    // Main loop.
    for (;;) {
        // Process all inputs.
        for (;;) {
            struct epoll_event event;
            int32_t status = sys_epoll_pwait(window.epollFd, &event, 1, 0, NULL);
            if (status < 0) return -1;
            if (status == 0) break;

            int32_t eventFd = *((int32_t *)event.data.ptr);
            if (eventFd == window.x11Client.socketFd) {
                int32_t numRead = x11Client_receive(&window.x11Client);
                if (numRead == 0) return 0;
                if (numRead <= 0) return -2;

                // Handle all received messages.
                for (;;) {
                    int32_t msgLength = x11Client_nextMessage(&window.x11Client);
                    if (msgLength == 0) break;

                    struct x11_genericResponse *generic = (void *)&window.x11Client.receiveBuffer[0];
                    printf("Got message type: %d, length: %d\n", (int32_t)generic->type, msgLength);
                    switch (generic->type & x11_TYPE_MASK) {
                        case x11_TYPE_ERROR: {
                            printf("X11 request failed (seq=%d, code=%d)\n", (int32_t)generic->sequenceNumber, (int32_t)generic->extra);
                            return -3; // For now we always exit on X11 errors.
                        }
                        case x11_configureNotify_TYPE: {
                            struct x11_configureNotify *configureNotify = (void *)generic;
                            game_onResize(configureNotify->width, configureNotify->height);
                            break;
                        }
                        case x11_genericEvent_TYPE: {
                            struct x11_xinputRawEvent *rawEvent = (void *)generic;
                            if (rawEvent->extension != window.xinputMajorOpcode || rawEvent->eventType != x11_XINPUT_RAW_MOTION) break;

                            int32_t valuatorBits = 0;
                            for (int32_t i = 0; i < rawEvent->numValuators; ++i) {
                                valuatorBits += hc_POPCOUNT32(rawEvent->data[i]);
                            }
                            struct x11_xinputFP3232 *valuatorsRaw = (void *)&rawEvent->data[rawEvent->numValuators + 2 * valuatorBits];

                            int64_t deltaX = (int64_t)((uint64_t)valuatorsRaw[0].integer << 32) | valuatorsRaw[0].fraction;
                            int64_t deltaY = (int64_t)((uint64_t)valuatorsRaw[1].integer << 32) | valuatorsRaw[1].fraction;
                            game_onMouseMove(deltaX, deltaY);
                            break;
                        }
                        case x11_clientMessage_TYPE: {
                            struct x11_clientMessage *message = (void *)generic;
                            if (message->atom == window.wmProtocolsAtom && message->data32[0] == window.wmDeleteWindowAtom) return 0;
                            break;
                        }
                    }
                    x11Client_ackMessage(&window.x11Client, msgLength);
                }
            }
        }
        // Rendering.
        if (game_draw() < 0 || !egl_swapBuffers(&window.egl)) return -4;

        ++frameCounter;
        struct timespec now;
        debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &now), == 0);
        if (now.tv_sec > prev.tv_sec) {
            printf("FPS: %llu\n", frameCounter);
            frameCounter = 0;
            prev = now;
        }
    }
}

static void window_deinit(void) {
    game_deinit();
    debug_CHECK(sys_close(window.epollFd), == 0);
    x11Client_deinit(&window.x11Client);
    egl_deinit(&window.egl);
}
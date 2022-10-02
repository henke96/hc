#define window_DEFAULT_WIDTH 640
#define window_DEFAULT_HEIGHT 480

struct window {
    struct x11Client x11Client;
    struct egl egl;
    struct x11_getKeyboardMappingResponse *keyboardMap;
    struct x11_getModifierMappingResponse *modifierMap;
    uint32_t keyboardMapSize;
    int32_t epollFd;
    uint32_t windowId;
    uint32_t rootWindowId;
    uint32_t wmProtocolsAtom;
    uint32_t wmDeleteWindowAtom;
    uint32_t wmStateAtom;
    uint32_t wmStateFullscreenAtom;
    uint32_t wmBypassCompositorAtom;
    uint8_t xinputMajorOpcode;
    uint8_t xfixesMajorOpcode;
    bool pointerGrabbed;
    char __pad[1];
};

static struct window window;

static int32_t window_x11Setup(uint32_t visualId) {
    window.windowId = x11Client_nextId(&window.x11Client);
    uint64_t rootsOffset = (
        math_ALIGN_FORWARD(window.x11Client.setupResponse->vendorLength, 4) +
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
        uint8_t queryXfixesPad[math_PAD_BYTES(sizeof(x11_XFIXES_NAME) - 1, 4)];
        struct x11_queryExtension queryXinput;
        char queryXinputName[sizeof(x11_XINPUT_NAME) - 1];
        uint8_t queryXinputPad[math_PAD_BYTES(sizeof(x11_XINPUT_NAME) - 1, 4)];
        struct x11_internAtom wmProtocolsAtom;
        char wmProtocolsAtomName[sizeof("WM_PROTOCOLS") - 1];
        uint8_t wmProtocolsAtomPad[math_PAD_BYTES(sizeof("WM_PROTOCOLS") - 1, 4)];
        struct x11_internAtom wmDeleteWindowAtom;
        char wmDeleteWindowAtomName[sizeof("WM_DELETE_WINDOW") - 1];
        uint8_t wmDeleteWindowAtomPad[math_PAD_BYTES(sizeof("WM_DELETE_WINDOW") - 1, 4)];
        struct x11_internAtom wmStateAtom;
        char wmStateAtomName[sizeof("_NET_WM_STATE") - 1];
        uint8_t wmStateAtomPad[math_PAD_BYTES(sizeof("_NET_WM_STATE") - 1, 4)];
        struct x11_internAtom wmStateFullscreenAtom;
        char wmStateFullscreenAtomName[sizeof("_NET_WM_STATE_FULLSCREEN") - 1];
        uint8_t wmStateFullscreenAtomPad[math_PAD_BYTES(sizeof("_NET_WM_STATE_FULLSCREEN") - 1, 4)];
        struct x11_internAtom wmBypassCompositorAtom;
        char wmBypassCompositorAtomName[sizeof("_NET_WM_BYPASS_COMPOSITOR") - 1];
        uint8_t wmBypassCompositorAtomPad[math_PAD_BYTES(sizeof("_NET_WM_BYPASS_COMPOSITOR") - 1, 4)];
        struct x11_getKeyboardMapping getKeyboardMapping;
        struct x11_getModifierMapping getModifierMapping;
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
            (
                x11_EVENT_STRUCTURE_NOTIFY_BIT |
                x11_EVENT_FOCUS_CHANGE_BIT |
                x11_EVENT_BUTTON_PRESS_BIT |
                x11_EVENT_BUTTON_RELEASE_BIT |
                x11_EVENT_KEY_PRESS_BIT |
                x11_EVENT_KEY_RELEASE_BIT
            )
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
        .wmDeleteWindowAtomName = "WM_DELETE_WINDOW",
        .wmStateAtom = {
            .opcode = x11_internAtom_OPCODE,
            .onlyIfExists = 0,
            .length = (sizeof(windowRequests.wmStateAtom) + sizeof(windowRequests.wmStateAtomName) + sizeof(windowRequests.wmStateAtomPad)) / 4,
            .nameLength = sizeof(windowRequests.wmStateAtomName)
        },
        .wmStateAtomName = "_NET_WM_STATE",
        .wmStateFullscreenAtom = {
            .opcode = x11_internAtom_OPCODE,
            .onlyIfExists = 0,
            .length = (sizeof(windowRequests.wmStateFullscreenAtom) + sizeof(windowRequests.wmStateFullscreenAtomName) + sizeof(windowRequests.wmStateFullscreenAtomPad)) / 4,
            .nameLength = sizeof(windowRequests.wmStateFullscreenAtomName)
        },
        .wmStateFullscreenAtomName = "_NET_WM_STATE_FULLSCREEN",
        .wmBypassCompositorAtom = {
            .opcode = x11_internAtom_OPCODE,
            .onlyIfExists = 0,
            .length = (sizeof(windowRequests.wmBypassCompositorAtom) + sizeof(windowRequests.wmBypassCompositorAtomName) + sizeof(windowRequests.wmBypassCompositorAtomPad)) / 4,
            .nameLength = sizeof(windowRequests.wmBypassCompositorAtomName)
        },
        .wmBypassCompositorAtomName = "_NET_WM_BYPASS_COMPOSITOR",
        .getKeyboardMapping = {
            .opcode = x11_getKeyboardMapping_OPCODE,
            .length = sizeof(windowRequests.getKeyboardMapping) / 4,
            .firstKeycode = window.x11Client.setupResponse->minKeycode,
            .count = 1 + window.x11Client.setupResponse->maxKeycode - window.x11Client.setupResponse->minKeycode
        },
        .getModifierMapping = {
            .opcode = x11_getModifierMapping_OPCODE,
            .length = sizeof(windowRequests.getModifierMapping) / 4
        }
    };
    int32_t status = x11Client_sendRequests(&window.x11Client, &windowRequests, sizeof(windowRequests), 11);
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
        // We only handle big responses for keyboard map.
        #define window_GETKEYBOARDMAPPING_SEQ 10
        if (msgLength < 0 && nextSequenceNumber != window_GETKEYBOARDMAPPING_SEQ) return -3;

        struct x11_genericResponse *generic = (void *)&window.x11Client.receiveBuffer[0];
        if (generic->type == x11_TYPE_ERROR) {
            debug_printNum("X11 request failed (seq=", (int32_t)generic->sequenceNumber, "\n");
            return -4;
        }

        if (generic->type == x11_TYPE_REPLY) {
            if (generic->sequenceNumber != nextSequenceNumber) return -5;

            switch (nextSequenceNumber) {
                case 3: {
                    struct x11_queryExtensionResponse *response = (void *)generic;
                    if (!response->present) return -6;
                    window.xfixesMajorOpcode = response->majorOpcode;
                    break;
                }
                case 4: {
                    struct x11_queryExtensionResponse *response = (void *)generic;
                    if (!response->present) return -7;
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
                    break;
                }
                case 7: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.wmStateAtom = response->atom;
                    break;
                }
                case 8: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.wmStateFullscreenAtom = response->atom;
                    break;
                }
                case 9: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.wmBypassCompositorAtom = response->atom;
                    break;
                }
                case window_GETKEYBOARDMAPPING_SEQ: {
                    window.keyboardMapSize = (uint32_t)hc_ABS32(msgLength);
                    window.keyboardMap = sys_mmap(NULL, window.keyboardMapSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
                    if ((int64_t)window.keyboardMap < 0) return -8;

                    // Handle big response.
                    if (msgLength < 0) {
                        uint32_t totalRead = window.x11Client.receiveLength;
                        while (totalRead < window.keyboardMapSize) {
                            int32_t read = (int32_t)sys_recvfrom(
                                window.x11Client.socketFd,
                                &((char *)window.keyboardMap)[totalRead],
                                window.keyboardMapSize - totalRead,
                                0, NULL, NULL
                            );
                            if (read <= 0) return -9;
                            totalRead += (uint32_t)read;
                        }

                        // Make msgLength normal again.
                        msgLength = (int32_t)window.x11Client.receiveLength;
                    }
                    hc_MEMCPY(window.keyboardMap, &window.x11Client.receiveBuffer[0], (uint32_t)msgLength);
                    break;
                }
                case 11: {
                    // TODO: Find MODE SWITCH modifier.
                    hc_UNUSED
                    struct x11_getModifierMappingResponse *response = (void *)generic;

                    // Done.
                    x11Client_ackMessage(&window.x11Client, msgLength);
                    return 0;
                }
                default: hc_UNREACHABLE;
            }
            ++nextSequenceNumber;
        }
        x11Client_ackMessage(&window.x11Client, msgLength);
    }
}

static int32_t window_init(char **envp) {
    window.pointerGrabbed = false;

    // Initialise EGL.
    int32_t status = egl_init(&window.egl);
    if (status < 0) {
        debug_printNum("Failed to initalise EGL (", status, ")\n");
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
        debug_printNum("Failed to create EGL context (", status, ")\n");
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
    struct xauth_entry entry = {0};
    if (xAuthorityFile != NULL && xauth_init(&xauth, xAuthorityFile) == 0) {
        xauth_nextEntry(&xauth, &entry);
        status = x11Client_init(&window.x11Client, &serverAddr, sizeof(serverAddr), &entry);
        xauth_deinit(&xauth);
    } else {
        status = x11Client_init(&window.x11Client, &serverAddr, sizeof(serverAddr), &entry);
    }
    if (status < 0) {
        debug_printNum("Failed to initialise x11Client (", status, ")\n");
        goto cleanup_egl;
    }

    status = window_x11Setup(eglVisualId);
    if (status < 0) {
        debug_printNum("X11 setup failed (", status, ")\n");
        goto cleanup_x11Client;
    }

    // Setup EGL surface.
    status = egl_setupSurface(&window.egl, (uint32_t)window.windowId);
    if (status < 0) {
        debug_printNum("Failed to setup EGL surface (", status, ")\n");
        goto cleanup_x11Setup;
    }
    debug_CHECK(egl_swapInterval(&window.egl, 0), RES == egl_TRUE);

    // Load OpenGL functions.
    status = gl_init(&window.egl);
    if (status < 0) {
        debug_printNum("Failed to initialise GL (", status, ")\n");
        goto cleanup_x11Setup;
    }

    // Initialise game.
    struct timespec initTimespec;
    debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &initTimespec), RES == 0);
    uint64_t initTimestamp = (uint64_t)initTimespec.tv_sec * 1000000000 + (uint64_t)initTimespec.tv_nsec;
    status = game_init(
        window_DEFAULT_WIDTH,
        window_DEFAULT_HEIGHT,
        initTimestamp
    );
    if (status < 0) {
        debug_printNum("Failed to initialise game (", status, ")\n");
        goto cleanup_x11Setup;
    }

    // Setup epoll.
    window.epollFd = sys_epoll_create1(0);
    if (window.epollFd < 0) {
        status = -6;
        goto cleanup_x11Setup;
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
    debug_CHECK(sys_close(window.epollFd), RES == 0);
    cleanup_x11Setup:
    debug_CHECK(sys_munmap(window.keyboardMap, window.keyboardMapSize), RES == 0);
    cleanup_x11Client:
    x11Client_deinit(&window.x11Client);
    cleanup_egl:
    egl_deinit(&window.egl);
    return status;
}

static int32_t window_grabPointer(void) {
    struct requests {
        struct x11_grabPointer grabPointer;
        struct x11_xfixesHideCursor hideCursor;
        struct x11_xinputSelectEvents xinputSelectEvents;
        struct x11_xinputEventMask xinputSelectEventsMask;
    };

    struct requests requests = {
        .grabPointer = {
            .opcode = x11_grabPointer_OPCODE,
            .ownerEvents = x11_TRUE,
            .length = sizeof(requests.grabPointer) / 4,
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
            .length = sizeof(requests.hideCursor) / 4,
            .windowId = window.windowId
        },
        .xinputSelectEvents = {
            .majorOpcode = window.xinputMajorOpcode,
            .opcode = x11_xinputSelectEvents_OPCODE,
            .length = (sizeof(requests.xinputSelectEvents) + sizeof(requests.xinputSelectEventsMask)) / 4,
            .windowId = window.rootWindowId,
            .numMasks = 1
        },
        .xinputSelectEventsMask = {
            .deviceId = x11_XINPUT_ALL_MASTER_DEVICES,
            .maskLength = 1,
            .mask = (1 << x11_XINPUT_RAW_MOTION)
        }
    };
    if (x11Client_sendRequests(&window.x11Client, &requests, sizeof(requests), 3) < 0) return -1;
    window.pointerGrabbed = true;
    return 0;
}

static int32_t window_ungrabPointer(void) {
    struct requests {
        struct x11_ungrabPointer ungrabPointer;
        struct x11_xfixesShowCursor showCursor;
        struct x11_xinputSelectEvents xinputSelectEvents;
        struct x11_xinputEventMask xinputSelectEventsMask;
    };

    struct requests requests = {
        .ungrabPointer = {
            .opcode = x11_ungrabPointer_OPCODE,
            .length = sizeof(requests.ungrabPointer) / 4,
            .time = 0 // CurrentTime
        },
        .showCursor = {
            .majorOpcode = window.xfixesMajorOpcode,
            .opcode = x11_xfixesShowCursor_OPCODE,
            .length = sizeof(requests.showCursor) / 4,
            .windowId = window.windowId
        },
        .xinputSelectEvents = {
            .majorOpcode = window.xinputMajorOpcode,
            .opcode = x11_xinputSelectEvents_OPCODE,
            .length = (sizeof(requests.xinputSelectEvents) + sizeof(requests.xinputSelectEventsMask)) / 4,
            .windowId = window.rootWindowId,
            .numMasks = 1
        },
        .xinputSelectEventsMask = {
            .deviceId = x11_XINPUT_ALL_MASTER_DEVICES,
            .maskLength = 1,
            .mask = 0
        }
    };
    if (x11Client_sendRequests(&window.x11Client, &requests, sizeof(requests), 3) < 0) return -1;
    window.pointerGrabbed = false;
    return 0;
}

static int32_t window_toggleFullscreen(void) {
    struct x11_sendEvent event = {
        .opcode = x11_sendEvent_OPCODE,
        .propagate = 0,
        .length = sizeof(event) / 4,
        .destWindowId = window.rootWindowId,
        .eventMask = x11_EVENT_SUBSTRUCTURE_NOTIFY_BIT | x11_EVENT_SUBSTRUCTURE_REDIRECT_BIT,
        .clientMessage = {
            .type = x11_clientMessage_TYPE,
            .format = 32,
            .sequenceNumber = 0,
            .window = window.windowId,
            .atom = window.wmStateAtom,
            .data32 = { x11_NET_WM_STATE_TOGGLE, window.wmStateFullscreenAtom, 0, 1, 0 }
        }
    };
    if (x11Client_sendRequests(&window.x11Client, &event, sizeof(event), 1) < 0) return -1;
    return 0;
}

static int32_t window_run(void) {
    struct requests {
        struct x11_changeProperty changeProperty;
        uint32_t changePropertyData;
        struct x11_changeProperty bypassCompositor;
        uint32_t bypassCompositorData;
        struct x11_xfixesQueryVersion queryXfixesVersion; // Need to do this once to tell server what version we expect.
    };
    struct requests setupRequests = {
        .changeProperty = {
            .opcode = x11_changeProperty_OPCODE,
            .mode = x11_changeProperty_REPLACE,
            .length = (sizeof(setupRequests.changeProperty) + sizeof(setupRequests.changePropertyData)) / 4,
            .window = window.windowId,
            .property = window.wmProtocolsAtom,
            .type = x11_ATOM_ATOM,
            .format = 32,
            .dataLength = 1
        },
        .changePropertyData = window.wmDeleteWindowAtom,
        .bypassCompositor = {
            .opcode = x11_changeProperty_OPCODE,
            .mode = x11_changeProperty_REPLACE,
            .length = (sizeof(setupRequests.bypassCompositor) + sizeof(setupRequests.bypassCompositorData)) / 4,
            .window = window.windowId,
            .property = window.wmBypassCompositorAtom,
            .type = x11_ATOM_CARDINAL,
            .format = 32,
            .dataLength = 1
        },
        .bypassCompositorData = 1,
        .queryXfixesVersion = {
            .majorOpcode = window.xfixesMajorOpcode,
            .opcode = x11_xfixesQueryVersion_OPCODE,
            .length = sizeof(setupRequests.queryXfixesVersion) / 4,
            .majorVersion = 4,
            .minorVersion = 0
        }
    };
    if (x11Client_sendRequests(&window.x11Client, &setupRequests, sizeof(setupRequests), 3) < 0) return -1;

    // Main loop.
    for (;;) {
        // Process all inputs.
        for (;;) {
            struct epoll_event event;
            event.data.ptr = NULL;
            int32_t status = sys_epoll_pwait(window.epollFd, &event, 1, 0, NULL);
            if (status < 0) return -1;
            if (status == 0) break;
            if (event.data.ptr == NULL) return -1; // Should never happen..

            struct timespec eventTimespec;
            debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &eventTimespec), RES == 0);
            uint64_t eventTimestamp = (uint64_t)eventTimespec.tv_sec * 1000000000 + (uint64_t)eventTimespec.tv_nsec;

            int32_t eventFd = *((int32_t *)event.data.ptr);
            if (eventFd == window.x11Client.socketFd) {
                int32_t numRead = x11Client_receive(&window.x11Client);
                if (numRead == 0) return 0;
                if (numRead <= 0) return -2;

                // Handle all received messages.
                for (;;) {
                    int32_t msgLength = x11Client_nextMessage(&window.x11Client);
                    if (msgLength == 0) break;
                    if (msgLength < 0) return -3;

                    struct x11_genericResponse *generic = (void *)&window.x11Client.receiveBuffer[0];
                    int32_t type = generic->type & x11_TYPE_MASK;
                    switch (type) {
                        case x11_TYPE_ERROR: {
                            debug_printNum("X11 request failed (code=", (int32_t)generic->extra, "\n");
                            return -4; // For now we always exit on X11 errors.
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
                            game_onMouseMove(deltaX, deltaY, eventTimestamp);
                            break;
                        }
                        case x11_clientMessage_TYPE: {
                            struct x11_clientMessage *message = (void *)generic;
                            if (message->atom == window.wmProtocolsAtom && message->data32[0] == window.wmDeleteWindowAtom) return 0;
                            break;
                        }
                        case x11_buttonPress_TYPE: {
                            if (!window.pointerGrabbed && window_grabPointer() < 0) return -5;
                            break;
                        }
                        case x11_keyPress_TYPE:
                        case x11_keyRelease_TYPE: {
                            struct x11_keyPress *message = (void *)generic; // Press/release structs are identical.
                            if (message->detail > 0x7F) break;

                            int32_t key = input_codeToKey[message->detail];
                            if (type == x11_keyPress_TYPE) {
                                if (key != 0) game_onKeyDown(key, eventTimestamp);
                                else if (message->detail == 0x09) { // Escape.
                                  if (window.pointerGrabbed && window_ungrabPointer() < 0) return -6;
                                } else if (message->detail == 0x5F) { // F11.
                                    if (window_toggleFullscreen() < 0) return -7;
                                }
                            } else if (key != 0) game_onKeyUp(key, eventTimestamp);
                            break;
                        }
                        case x11_focusOut_TYPE: {
                            if (window.pointerGrabbed && window_ungrabPointer() < 0) return -8;
                            break;
                        }
                    }
                    x11Client_ackMessage(&window.x11Client, msgLength);
                }
            }
        }
        // Rendering.
        struct timespec drawTimespec;
        debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &drawTimespec), RES == 0);
        uint64_t drawTimestamp = (uint64_t)drawTimespec.tv_sec * 1000000000 + (uint64_t)drawTimespec.tv_nsec;
        if (game_draw(drawTimestamp) < 0 || !egl_swapBuffers(&window.egl)) return -9;
    }
}

static void window_deinit(void) {
    game_deinit();
    debug_CHECK(sys_close(window.epollFd), RES == 0);
    debug_CHECK(sys_munmap(window.keyboardMap, window.keyboardMapSize), RES == 0);
    x11Client_deinit(&window.x11Client);
    egl_deinit(&window.egl);
}
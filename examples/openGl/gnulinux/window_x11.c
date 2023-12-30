static int32_t window_x11_setup(uint32_t visualId) {
    window.x11.windowId = x11Client_nextId(&window.x11.client);

    char *vendor = (void *)&window.x11.client.setupResponse[1];
    struct x11_format *pixmapFormats = (void *)&vendor[math_ALIGN_FORWARD(window.x11.client.setupResponse->vendorLength, 4)];
    struct x11_screen *roots = (void *)&pixmapFormats[window.x11.client.setupResponse->numPixmapFormats];
    window.x11.rootWindowId = roots[0].windowId; // Use screen 0.

    struct requests {
        struct x11_createWindow createWindow;
        uint32_t createWindowValues[1];
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
        struct x11_internAtom motifWmHintsAtom;
        char motifWmHintsAtomName[sizeof("_MOTIF_WM_HINTS") - 1];
        uint8_t motifWmHintsAtomPad[math_PAD_BYTES(sizeof("_MOTIF_WM_HINTS") - 1, 4)];
        struct x11_getKeyboardMapping getKeyboardMapping;
        struct x11_getModifierMapping getModifierMapping;
    };

    struct requests windowRequests = {
        .createWindow = {
            .opcode = x11_createWindow_OPCODE,
            .depth = 0,
            .length = (sizeof(windowRequests.createWindow) + sizeof(windowRequests.createWindowValues)) / 4,
            .windowId = window.x11.windowId,
            .parentId = window.x11.rootWindowId,
            .width = window.width,
            .height = window.height,
            .borderWidth = 0,
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
        .motifWmHintsAtom = {
            .opcode = x11_internAtom_OPCODE,
            .onlyIfExists = 0,
            .length = (sizeof(windowRequests.motifWmHintsAtom) + sizeof(windowRequests.motifWmHintsAtomName) + sizeof(windowRequests.motifWmHintsAtomPad)) / 4,
            .nameLength = sizeof(windowRequests.motifWmHintsAtomName)
        },
        .motifWmHintsAtomName = "_MOTIF_WM_HINTS",
        .getKeyboardMapping = {
            .opcode = x11_getKeyboardMapping_OPCODE,
            .length = sizeof(windowRequests.getKeyboardMapping) / 4,
            .firstKeycode = window.x11.client.setupResponse->minKeycode,
            .count = 1 + window.x11.client.setupResponse->maxKeycode - window.x11.client.setupResponse->minKeycode
        },
        .getModifierMapping = {
            .opcode = x11_getModifierMapping_OPCODE,
            .length = sizeof(windowRequests.getModifierMapping) / 4
        }
    };
    int32_t status = x11Client_sendRequests(&window.x11.client, &windowRequests, sizeof(windowRequests), 11);
    if (status < 0) return -1;

    // Wait for all replies.
    int32_t nextSequenceNumber = 2;
    for (;;) {
        int32_t msgSize = x11Client_nextMessage(&window.x11.client);
        if (msgSize == 0) {
            int32_t numRead = x11Client_receive(&window.x11.client);
            if (numRead <= 0) return -2;
            continue;
        }
        // We only handle big responses for keyboard map.
        #define window_GETKEYBOARDMAPPING_SEQ 10
        if (msgSize < 0 && nextSequenceNumber != window_GETKEYBOARDMAPPING_SEQ) return -3;

        struct x11_genericResponse *generic = (void *)&window.x11.client.buffer[window.x11.client.bufferPos];
        if (generic->type == x11_TYPE_ERROR) {
            debug_printNum("X11 request failed (seq=", (int32_t)generic->sequenceNumber, "\n");
            return -4;
        }

        if (generic->type == x11_TYPE_REPLY) {
            if (generic->sequenceNumber != nextSequenceNumber) return -5;

            switch (nextSequenceNumber) {
                case 2: {
                    struct x11_queryExtensionResponse *response = (void *)generic;
                    if (!response->present) return -6;
                    window.x11.xfixesMajorOpcode = response->majorOpcode;
                    break;
                }
                case 3: {
                    struct x11_queryExtensionResponse *response = (void *)generic;
                    if (!response->present) return -7;
                    window.x11.xinputMajorOpcode = response->majorOpcode;
                    break;
                }
                case 4: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.x11.wmProtocolsAtom = response->atom;
                    break;
                }
                case 5: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.x11.wmDeleteWindowAtom = response->atom;
                    break;
                }
                case 6: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.x11.wmStateAtom = response->atom;
                    break;
                }
                case 7: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.x11.wmStateFullscreenAtom = response->atom;
                    break;
                }
                case 8: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.x11.wmBypassCompositorAtom = response->atom;
                    break;
                }
                case 9: {
                    struct x11_internAtomResponse *response = (void *)generic;
                    window.x11.motifWmHintsAtom = response->atom;
                    break;
                }
                case window_GETKEYBOARDMAPPING_SEQ: {
                    window.x11.keyboardMapSize = hc_ABS32(msgSize);
                    window.x11.keyboardMap = sys_mmap(NULL, window.x11.keyboardMapSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
                    if ((int64_t)window.x11.keyboardMap < 0) return -8;

                    // Handle big response.
                    if (msgSize < 0) {
                        int32_t totalRead = window.x11.client.receivedSize;
                        while (totalRead < window.x11.keyboardMapSize) {
                            int32_t read = (int32_t)sys_recvfrom(
                                window.x11.client.socketFd,
                                (char *)window.x11.keyboardMap + totalRead,
                                window.x11.keyboardMapSize - totalRead,
                                0, NULL, NULL
                            );
                            if (read <= 0) return -9;
                            totalRead += read;
                        }

                        // Make msgSize normal again.
                        msgSize = window.x11.client.receivedSize;
                    }
                    hc_MEMCPY(window.x11.keyboardMap, &window.x11.client.buffer[window.x11.client.bufferPos], (uint64_t)msgSize);
                    break;
                }
                case 11: {
                    // TODO: Find MODE SWITCH modifier.
                    hc_UNUSED
                    struct x11_getModifierMappingResponse *response = (void *)generic;

                    // Done.
                    x11Client_ackMessage(&window.x11.client, msgSize);
                    return 0;
                }
                default: hc_UNREACHABLE;
            }
            ++nextSequenceNumber;
        }
        x11Client_ackMessage(&window.x11.client, msgSize);
    }
}

static int32_t window_x11_init(void **eglWindow, char **envp) {
    uint64_t platformAttrs[] = {
        egl_PLATFORM_XCB_SCREEN_EXT, 0, // We always use screen 0.
        egl_NONE
    };
    int32_t status = egl_createContext(
        &window.egl,
        egl_PLATFORM_XCB_EXT,
        &platformAttrs[0],
        egl_DEFAULT_DISPLAY,
        egl_OPENGL_ES_API,
        &window_configAttributes[0],
        &window_contextAttributes[0]
    );
    if (status < 0) {
        debug_printNum("Failed to initialise EGL context (", status, ")\n");
        return -1;
    }
    uint32_t eglVisualId = (uint32_t)status;

    // Initialise x11.
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    const static char address[] = "/tmp/.X11-unix/X0";
    hc_MEMCPY(&serverAddr.sun_path[0], &address[0], sizeof(address));

    // Parse DISPLAY.
    char *xDisplay = util_getEnv(envp, "DISPLAY");
    if (xDisplay != NULL && xDisplay[0] == ':') {
        ++xDisplay; // Skip colon.
        int64_t prefixLen = sizeof("/tmp/.X11-unix/X") - 1;
        int64_t maxNum = (int64_t)sizeof(serverAddr.sun_path) - prefixLen - 1; // Reserve space for prefix and null terminator.

        int64_t i = 0;
        for (; i < maxNum; ++i) {
            int32_t c = xDisplay[i];
            if (c < '0' || c > '9') break;
            serverAddr.sun_path[prefixLen + i] = (char)c;
        }
        serverAddr.sun_path[prefixLen + i] = '\0';
    }

    char *xAuthorityFile = util_getEnv(envp, "XAUTHORITY");
    struct xauth xauth;
    struct xauth_entry entry = {0};
    if (xAuthorityFile != NULL && xauth_init(&xauth, xAuthorityFile) == 0) {
        xauth_nextEntry(&xauth, &entry);
        status = x11Client_init(&window.x11.client, &serverAddr, sizeof(serverAddr), &entry);
        xauth_deinit(&xauth);
    } else {
        status = x11Client_init(&window.x11.client, &serverAddr, sizeof(serverAddr), &entry);
    }
    if (status < 0) {
        debug_printNum("Failed to initialise x11Client (", status, ")\n");
        goto cleanup_eglContext;
    }

    status = window_x11_setup(eglVisualId);
    if (status < 0) {
        debug_printNum("X11 setup failed (", status, ")\n");
        goto cleanup_x11Client;
    }

    struct epoll_event x11SocketEvent = {
        .events = EPOLLIN,
        .data.ptr = &window.x11.client.socketFd
    };
    if (sys_epoll_ctl(window.epollFd, EPOLL_CTL_ADD, window.x11.client.socketFd, &x11SocketEvent) < 0) {
        goto cleanup_x11Setup;
    }
    *eglWindow = (void *)(size_t)window.x11.windowId;
    return 0;

    cleanup_x11Setup:
    debug_CHECK(sys_munmap(window.x11.keyboardMap, window.x11.keyboardMapSize), RES == 0);
    cleanup_x11Client:
    x11Client_deinit(&window.x11.client);
    cleanup_eglContext:
    egl_destroyContext(&window.egl);
    return -1;
}

static int32_t window_x11_grabPointer(void) {
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
            .grabWindowId = window.x11.windowId,
            .eventMask = x11_EVENT_BUTTON_PRESS_BIT | x11_EVENT_BUTTON_RELEASE_BIT,
            .pointerMode = x11_grabPointer_ASYNCHRONOUS,
            .keyboardMode = x11_grabPointer_ASYNCHRONOUS,
            .confineToWindowId = window.x11.windowId,
            .cursor = 0,
            .time = 0 // CurrentTime
        },
        .hideCursor = {
            .majorOpcode = window.x11.xfixesMajorOpcode,
            .opcode = x11_xfixesHideCursor_OPCODE,
            .length = sizeof(requests.hideCursor) / 4,
            .windowId = window.x11.windowId
        },
        .xinputSelectEvents = {
            .majorOpcode = window.x11.xinputMajorOpcode,
            .opcode = x11_xinputSelectEvents_OPCODE,
            .length = (sizeof(requests.xinputSelectEvents) + sizeof(requests.xinputSelectEventsMask)) / 4,
            .windowId = window.x11.rootWindowId,
            .numMasks = 1
        },
        .xinputSelectEventsMask = {
            .deviceId = x11_XINPUT_ALL_MASTER_DEVICES,
            .maskLength = 1,
            .mask = (1 << x11_XINPUT_RAW_MOTION)
        }
    };
    if (x11Client_sendRequests(&window.x11.client, &requests, sizeof(requests), 3) < 0) return -1;
    window.pointerGrabbed = true;
    return 0;
}

static int32_t window_x11_ungrabPointer(void) {
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
            .majorOpcode = window.x11.xfixesMajorOpcode,
            .opcode = x11_xfixesShowCursor_OPCODE,
            .length = sizeof(requests.showCursor) / 4,
            .windowId = window.x11.windowId
        },
        .xinputSelectEvents = {
            .majorOpcode = window.x11.xinputMajorOpcode,
            .opcode = x11_xinputSelectEvents_OPCODE,
            .length = (sizeof(requests.xinputSelectEvents) + sizeof(requests.xinputSelectEventsMask)) / 4,
            .windowId = window.x11.rootWindowId,
            .numMasks = 1
        },
        .xinputSelectEventsMask = {
            .deviceId = x11_XINPUT_ALL_MASTER_DEVICES,
            .maskLength = 1,
            .mask = 0
        }
    };
    if (x11Client_sendRequests(&window.x11.client, &requests, sizeof(requests), 3) < 0) return -1;
    window.pointerGrabbed = false;
    return 0;
}

static int32_t window_x11_toggleFullscreen(void) {
    struct x11_sendEvent event = {
        .opcode = x11_sendEvent_OPCODE,
        .propagate = 0,
        .length = sizeof(event) / 4,
        .destWindowId = window.x11.rootWindowId,
        .eventMask = x11_EVENT_SUBSTRUCTURE_NOTIFY_BIT | x11_EVENT_SUBSTRUCTURE_REDIRECT_BIT,
        .clientMessage = {
            .type = x11_clientMessage_TYPE,
            .format = 32,
            .sequenceNumber = 0,
            .window = window.x11.windowId,
            .atom = window.x11.wmStateAtom,
            .data32 = { x11_NET_WM_STATE_TOGGLE, window.x11.wmStateFullscreenAtom, 0, 1, 0 }
        }
    };
    if (x11Client_sendRequests(&window.x11.client, &event, sizeof(event), 1) < 0) return -1;
    return 0;
}

static int32_t window_x11_run(void) {
    struct requests {
        struct x11_changeProperty changeProperty;
        uint32_t changePropertyData;
        struct x11_changeProperty bypassCompositor;
        uint32_t bypassCompositorData;
        struct x11_changeProperty disableDecorations;
        struct x11_motifWmHints disableDecorationsData;
        struct x11_xfixesQueryVersion queryXfixesVersion; // Need to do this once to tell server what version we expect.
        struct x11_mapWindow mapWindow;
    };
    struct requests setupRequests = {
        .changeProperty = {
            .opcode = x11_changeProperty_OPCODE,
            .mode = x11_changeProperty_REPLACE,
            .length = (sizeof(setupRequests.changeProperty) + sizeof(setupRequests.changePropertyData)) / 4,
            .window = window.x11.windowId,
            .property = window.x11.wmProtocolsAtom,
            .type = x11_ATOM_ATOM,
            .format = 32,
            .dataLength = 1
        },
        .changePropertyData = window.x11.wmDeleteWindowAtom,
        .bypassCompositor = {
            .opcode = x11_changeProperty_OPCODE,
            .mode = x11_changeProperty_REPLACE,
            .length = (sizeof(setupRequests.bypassCompositor) + sizeof(setupRequests.bypassCompositorData)) / 4,
            .window = window.x11.windowId,
            .property = window.x11.wmBypassCompositorAtom,
            .type = x11_ATOM_CARDINAL,
            .format = 32,
            .dataLength = 1
        },
        .bypassCompositorData = 1,
        .disableDecorations = {
            .opcode = x11_changeProperty_OPCODE,
            .mode = x11_changeProperty_REPLACE,
            .length = (sizeof(setupRequests.disableDecorations) + sizeof(setupRequests.disableDecorationsData)) / 4,
            .window = window.x11.windowId,
            .property = window.x11.motifWmHintsAtom,
            .type = window.x11.motifWmHintsAtom,
            .format = 32,
            .dataLength = sizeof(setupRequests.disableDecorationsData) / 4
        },
        .disableDecorationsData = {
            .flags = x11_motifWmHints_DECORATIONS,
            .decorations = 0
        },
        .queryXfixesVersion = {
            .majorOpcode = window.x11.xfixesMajorOpcode,
            .opcode = x11_xfixesQueryVersion_OPCODE,
            .length = sizeof(setupRequests.queryXfixesVersion) / 4,
            .majorVersion = 4,
            .minorVersion = 0
        },
        .mapWindow = {
            .opcode = x11_mapWindow_OPCODE,
            .length = sizeof(setupRequests.mapWindow) / 4,
            .windowId = window.x11.windowId
        },
    };
    if (x11Client_sendRequests(&window.x11.client, &setupRequests, sizeof(setupRequests), 5) < 0) return -1;

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
            if (eventFd == window.x11.client.socketFd) {
                int32_t numRead = x11Client_receive(&window.x11.client);
                if (numRead == 0) return 0;
                if (numRead <= 0) return -2;

                // Handle all received messages.
                for (;;) {
                    int32_t msgSize = x11Client_nextMessage(&window.x11.client);
                    if (msgSize == 0) break;
                    if (msgSize < 0) return -3;

                    struct x11_genericResponse *generic = (void *)&window.x11.client.buffer[window.x11.client.bufferPos];
                    int32_t type = generic->type & x11_TYPE_MASK;
                    switch (type) {
                        case x11_TYPE_ERROR: {
                            debug_printNum("X11 request failed (code=", (int32_t)generic->extra, "\n");
                            return -4; // For now we always exit on X11 errors.
                        }
                        case x11_configureNotify_TYPE: {
                            struct x11_configureNotify *configureNotify = (void *)generic;
                            window.width = configureNotify->width;
                            window.height = configureNotify->height;
                            game_onResize(window.width, window.height);
                            break;
                        }
                        case x11_genericEvent_TYPE: {
                            struct x11_xinputRawEvent *rawEvent = (void *)generic;
                            if (rawEvent->extension != window.x11.xinputMajorOpcode || rawEvent->eventType != x11_XINPUT_RAW_MOTION) break;

                            uint32_t *valuators = (void *)&rawEvent[1];
                            int32_t valuatorBits = 0;
                            for (int32_t i = 0; i < rawEvent->numValuators; ++i) {
                                valuatorBits += hc_POPCOUNT32(valuators[i]);
                            }
                            struct x11_xinputFP3232 *axisValues = (void *)&valuators[rawEvent->numValuators];
                            struct x11_xinputFP3232 *axisValuesRaw = &axisValues[valuatorBits];

                            int64_t deltaX = (int64_t)((uint64_t)axisValuesRaw[0].integer << 32) | axisValuesRaw[0].fraction;
                            int64_t deltaY = (int64_t)((uint64_t)axisValuesRaw[1].integer << 32) | axisValuesRaw[1].fraction;
                            game_onMouseMove(deltaX, deltaY, eventTimestamp);
                            break;
                        }
                        case x11_clientMessage_TYPE: {
                            struct x11_clientMessage *message = (void *)generic;
                            if (message->atom == window.x11.wmProtocolsAtom && message->data32[0] == window.x11.wmDeleteWindowAtom) return 0;
                            break;
                        }
                        case x11_buttonPress_TYPE: {
                            if (!window.pointerGrabbed && window_x11_grabPointer() < 0) return -5;
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
                                  if (window.pointerGrabbed && window_x11_ungrabPointer() < 0) return -6;
                                } else if (message->detail == 0x5F) { // F11.
                                    if (window_x11_toggleFullscreen() < 0) return -7;
                                }
                            } else if (key != 0) game_onKeyUp(key, eventTimestamp);
                            break;
                        }
                        case x11_focusOut_TYPE: {
                            if (window.pointerGrabbed && window_x11_ungrabPointer() < 0) return -8;
                            break;
                        }
                    }
                    x11Client_ackMessage(&window.x11.client, msgSize);
                }
            }
        }
        // Rendering.
        struct timespec drawTimespec;
        debug_CHECK(clock_gettime(CLOCK_MONOTONIC, &drawTimespec), RES == 0);
        uint64_t drawTimestamp = (uint64_t)drawTimespec.tv_sec * 1000000000 + (uint64_t)drawTimespec.tv_nsec;
        if (game_draw(drawTimestamp) < 0) return -9;
        debug_CHECK(egl_swapBuffers(&window.egl), RES == 1);
    }
}

static void window_x11_deinit(void) {
    debug_CHECK(sys_munmap(window.x11.keyboardMap, window.x11.keyboardMapSize), RES == 0);
    x11Client_deinit(&window.x11.client);
    egl_destroyContext(&window.egl);
}

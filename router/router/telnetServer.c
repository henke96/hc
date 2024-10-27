struct telnetServer {
    int32_t pidFd;
    int32_t listenFd;
    int32_t clientFd;
    int32_t ptMasterFd;
    uint8_t saved[12]; // Worst case command is: IAC SB NAWS IAC 255 IAC 255 IAC 255 IAC 255 IAC SE
};

static struct telnetServer telnetServer;

static noreturn void _telnetServer_shell(hc_UNUSED void *arg) {
    if (
        sys_close_range(4, INT32_MAX, CLOSE_RANGE_UNSHARE) == 0 &&
        sys_dup3(3, 0, 0) == 0 &&
        sys_dup3(3, 1, 0) == 1 &&
        sys_dup3(3, 2, 0) == 2
    ) {
        const char *argv[] = { "/shell", NULL };
        const char *envp[] = { "HOME=/", "TERM=linux", NULL };
        sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
    }
    sys_exit_group(1);
}

static void telnetServer_init(void) {
    telnetServer.clientFd = -1;

    telnetServer.ptMasterFd = sys_openat(-1, "/devpts/ptmx", O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
    CHECK(telnetServer.ptMasterFd, RES > 0);
    int32_t zero = 0;
    CHECK(sys_ioctl(telnetServer.ptMasterFd, TIOCSPTLCK, &zero), RES == 0);
    int32_t ptSlaveFd = sys_ioctl(telnetServer.ptMasterFd, TIOCGPTPEER, (void *)(O_RDWR | O_NOCTTY));
    CHECK(ptSlaveFd, RES > 0);

    CHECK(sys_dup3(ptSlaveFd, 3, O_CLOEXEC), RES == 3);
    CHECK(sys_close(ptSlaveFd), RES == 0);

    struct clone_args args = {
        .flags = CLONE_VM | CLONE_FILES | CLONE_VFORK | CLONE_PIDFD,
        .pidfd = &telnetServer.pidFd,
        .stack = &buffer[0],
        .stack_size = sizeof(buffer)
    };
    CHECK(sys_clone3_func(&args, sizeof(args), _telnetServer_shell, NULL), RES > 0);

    telnetServer.listenFd = sys_socket(AF_INET, SOCK_STREAM, 0);
    CHECK(telnetServer.listenFd, RES > 0);
    struct sockaddr_in listenAddr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(23),
        .sin_addr = { 10, 123, 0, 1 }
    };
    CHECK(sys_bind(telnetServer.listenFd, &listenAddr, sizeof(listenAddr)), RES == 0);
    CHECK(sys_listen(telnetServer.listenFd, 1), RES == 0);
}

static void _telnetServer_kickClient(void) {
    if (telnetServer.clientFd >= 0) {
        debug_CHECK(sys_close(telnetServer.clientFd), RES == 0);
        telnetServer.clientFd = -1;
    }
}

static void telnetServer_onListenFd(int32_t epollFd) {
    int32_t newClientFd = sys_accept4(telnetServer.listenFd, NULL, NULL, 0);
    debug_ASSERT(newClientFd > 0);
    if (newClientFd < 0) return;

    _telnetServer_kickClient();

    uint8_t handshake[] = {
        telnet_IAC, telnet_WILL, telnet_OPT_ECHO,
        telnet_IAC, telnet_WILL, telnet_OPT_SGA,
        telnet_IAC, telnet_DO, telnet_OPT_NAWS
    };
    if (sys_sendto(newClientFd, &handshake[0], sizeof(handshake), MSG_NOSIGNAL | MSG_DONTWAIT, NULL, 0) != sizeof(handshake)) {
        _telnetServer_kickClient();
        return;
    }

    hc_MEMSET(&telnetServer.saved[0], 0, sizeof(telnetServer.saved));
    telnetServer.clientFd = newClientFd;

    epollAdd(epollFd, telnetServer.clientFd);
}

static void telnetServer_onPtMasterFd(void) {
    int64_t numBuffered = sys_read(telnetServer.ptMasterFd, &buffer[0], sizeof(buffer));
    CHECK(numBuffered, RES > 0);
    if (telnetServer.clientFd < 0) return;

    int64_t baseI = 0;
    for (;;) {
        // Write until CR, NL, IAC or end of buffer.
        int64_t i = baseI;
        while (i < numBuffered && buffer[i] != '\r' && buffer[i] != '\n' && buffer[i] != telnet_IAC) ++i;

        int64_t toSend = i - baseI;
        if (toSend > 0 && sys_sendto(telnetServer.clientFd, &buffer[baseI], toSend, MSG_NOSIGNAL | MSG_DONTWAIT, NULL, 0) != toSend) {
            _telnetServer_kickClient();
            return;
        }
        if (i >= numBuffered) break;

        // Handle CR, NL or IAC.
        char bytes[2];
        switch (buffer[i]) {
            case '\r': {
                bytes[0] = '\r';
                bytes[1] = '\0';
                break;
            }
            case '\n': {
                bytes[0] = '\r';
                bytes[1] = '\n';
                break;
            }
            case telnet_IAC: {
                bytes[0] = telnet_IAC;
                bytes[1] = telnet_IAC;
                break;
            }
            default: hc_UNREACHABLE;
        }
        if (sys_sendto(telnetServer.clientFd, &bytes[0], sizeof(bytes), MSG_NOSIGNAL | MSG_DONTWAIT, NULL, 0) != sizeof(bytes)) {
            _telnetServer_kickClient();
            return;
        }
        baseI = i + 1;
    }
}

// Returns next index, or -1 if bytes were saved.
static int64_t _telnetServer_getByteOrSave(int64_t startI, int64_t currI, int64_t numBuffered, char *out) {
    if (currI >= numBuffered) {
        int64_t toSave = numBuffered - startI;
        debug_ASSERT(toSave <= (int64_t)hc_ARRAY_LEN(telnetServer.saved));
        hc_MEMCPY(&telnetServer.saved[(int64_t)hc_ARRAY_LEN(telnetServer.saved) - toSave], &buffer[startI], (size_t)toSave);
        return -1;
    }
    *out = buffer[currI];
    return currI + 1;
}

// Returns next index, or -1 if bytes were saved, or -2 if client should be kicked.
static int64_t _telnetServer_getNawsU16OrSave(int64_t startI, int64_t currI, int64_t numBuffered, uint16_t *out) {
    char msb;
    currI = _telnetServer_getByteOrSave(startI, currI, numBuffered, &msb);
    if (currI < 0) return -1;
    if (msb == telnet_IAC) {
        currI = _telnetServer_getByteOrSave(startI, currI, numBuffered, &msb);
        if (currI < 0) return -1;
        if (msb != telnet_IAC) return -2;
    }
    char lsb;
    currI = _telnetServer_getByteOrSave(startI, currI, numBuffered, &lsb);
    if (currI < 0) return -1;
    if (lsb == telnet_IAC) {
        currI = _telnetServer_getByteOrSave(startI, currI, numBuffered, &lsb);
        if (currI < 0) return -1;
        if (lsb != telnet_IAC) return -2;
    }
    *out = (uint16_t)(msb << 8) | lsb;
    return currI;
}

static void telnetServer_onClientFd(void) {
    int32_t numSaved = 0;
    for (int32_t i = 0; i < (int32_t)hc_ARRAY_LEN(telnetServer.saved); ++i) {
        if (telnetServer.saved[i] != 0) {
            numSaved = (int32_t)hc_ARRAY_LEN(telnetServer.saved) - i;
            hc_MEMCPY(&buffer[0], &telnetServer.saved[i], (size_t)numSaved);
            hc_MEMSET(&telnetServer.saved[i], 0, (size_t)numSaved);
            break;
        }
    }
    int64_t numRecv = sys_recvfrom(telnetServer.clientFd, &buffer[numSaved], (int32_t)sizeof(buffer) - numSaved, 0, NULL, NULL);
    if (numRecv <= 0) goto kickClient;
    int64_t numBuffered = numSaved + numRecv;
    int64_t baseI = 0;
    for (;;) {
        // Write until CR, IAC or end of buffer.
        int64_t i = baseI;
        while (i < numBuffered && buffer[i] != '\r' && buffer[i] != telnet_IAC) ++i;

        int64_t toWrite = i - baseI;
        if (toWrite > 0 && sys_write(telnetServer.ptMasterFd, &buffer[baseI], toWrite) != toWrite) goto kickClient;
        if (i >= numBuffered) goto doneWriting;

        // Handle CR or IAC.
        int64_t currI = i + 1;
        char followup;
        currI = _telnetServer_getByteOrSave(i, currI, numBuffered, &followup);
        if (currI < 0) goto doneWriting;

        if (buffer[i] == telnet_IAC) {
            switch (followup) {
                // Treat all 2 byte commands as NOPs.
                case telnet_NOP ... telnet_GO_AHEAD: break;
                case telnet_WILL ... telnet_DONT: {
                    char opt;
                    currI = _telnetServer_getByteOrSave(i, currI, numBuffered, &opt);
                    if (currI < 0) goto doneWriting;
                    break;
                }
                case telnet_IAC: {
                    if (sys_write(telnetServer.ptMasterFd, &buffer[i], 1) != 1) goto kickClient;
                    break;
                }
                case telnet_SB: {
                    char opt;
                    currI = _telnetServer_getByteOrSave(i, currI, numBuffered, &opt);
                    if (currI < 0) goto doneWriting;

                    switch (opt) {
                        case telnet_OPT_NAWS: {
                            struct winsize winsize = {0};
                            currI = _telnetServer_getNawsU16OrSave(i, currI, numBuffered, &winsize.ws_col);
                            if (currI < 0) {
                                if (currI < -1) goto kickClient;
                                goto doneWriting;
                            }
                            currI = _telnetServer_getNawsU16OrSave(i, currI, numBuffered, &winsize.ws_row);
                            if (currI < 0) {
                                if (currI < -1) goto kickClient;
                                goto doneWriting;
                            }

                            char iac;
                            currI = _telnetServer_getByteOrSave(i, currI, numBuffered, &iac);
                            if (currI < 0) goto doneWriting;
                            char se;
                            currI = _telnetServer_getByteOrSave(i, currI, numBuffered, &se);
                            if (currI < 0) goto doneWriting;
                            if (iac != telnet_IAC || se != telnet_SE) goto kickClient;
                            if (sys_ioctl(telnetServer.ptMasterFd, TIOCSWINSZ, &winsize) < 0) goto kickClient;
                            break;
                        }
                        default: goto kickClient;
                    }
                    break;
                }
                default: goto kickClient;
            }
        } else {
            debug_ASSERT(buffer[i] == '\r');

            bool valid = followup == '\0' || followup == '\n';
            if (!valid || sys_write(telnetServer.ptMasterFd, &buffer[i], 1) != 1) goto kickClient;
        }
        baseI = currI;
    }
    doneWriting:
    return;
    kickClient:
    _telnetServer_kickClient();
}

static void telnetServer_deinit(void) {
    _telnetServer_kickClient();
    debug_CHECK(sys_close(telnetServer.pidFd), RES == 0);
    debug_CHECK(sys_close(telnetServer.listenFd), RES == 0);
    debug_CHECK(sys_close(telnetServer.ptMasterFd), RES == 0);
}

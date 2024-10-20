#define modemClient_NO_CMD 0
#define modemClient_CONSTANT 1
#define modemClient_START_SMS 2
#define modemClient_SEND_SMS 3

struct modemClient_constant {
    uint8_t tag;
    uint16_t constantLength;
    const char *constant;
} hc_PACKED(1);
// Disable echo, set sms to text mode, enable extended sms info.
#define modemClient_INIT_CMD "ATE0;+CMGF=1;+CSDH=1;+CSCS=\"UCS2\"\r"
// Delete all messages except unread, list all messages.
#define modemClient_SMS_POLL_CMD "AT+CMGD=,3;+CMGL=\"REC UNREAD\"\r"

struct modemClient_startSms {
    uint8_t tag;
    uint8_t numberSize;
    // char number[];
};

struct modemClient_sendSms {
    uint8_t tag;
    uint16_t smsSize;
    // char sms[];
} hc_PACKED(1);

struct modemClient {
    const char *path;
    int32_t fd; // -1 if not connected.
    int32_t timerFd;
    int32_t receivedSize;
    int32_t commandQueueSize;
    char buffer[4096];
    char commandQueue[4103];
    bool pendingCommand;
};

static hc_INLINE uint8_t modemClient_pendingCmdTag(struct modemClient *self) {
    debug_ASSERT(!self->pendingCommand || self->commandQueueSize > 0);
    return self->pendingCommand ? self->commandQueue[0] : modemClient_NO_CMD;
}

static void modemClient_init(struct modemClient *self, const char *path) {
    self->path = path;
    self->fd = -1;
    self->timerFd = sys_timerfd_create(CLOCK_MONOTONIC, 0);
    CHECK(self->timerFd, RES > 0);

    struct itimerspec timeout = { .it_value = { .tv_sec = 20 } };
    CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
}

static void modemClient_disconnect(struct modemClient *self) {
    if (self->fd > 0) {
        sys_write(self->fd, hc_STR_COMMA_LEN("\x1b")); // Attempt to exit potential `> `-mode by sending escape.
        debug_printNum("AT disconnect (", modemClient_pendingCmdTag(self), ")\n");

        debug_CHECK(sys_close(self->fd), RES == 0);
        self->fd = -1;
    }
}

static int32_t modemClient_processQueue(struct modemClient *self) {
    if (self->pendingCommand || self->commandQueueSize == 0) return 0;

    uint8_t tag = self->commandQueue[0];
    switch (tag) {
        case modemClient_CONSTANT: {
            struct modemClient_constant *cmd = (void *)&self->commandQueue[0];
            int64_t written = sys_write(self->fd, cmd->constant, cmd->constantLength);
            if (written != cmd->constantLength) return -1;
            break;
        }
        case modemClient_START_SMS: {
            struct modemClient_startSms *cmd = (void *)&self->commandQueue[0];
            #define modemClient_START_SMS_CMD "AT+CMGS=\""
            char *number = (void *)&cmd[1];
            struct iovec_const iov[] = {
                { hc_STR_COMMA_LEN(modemClient_START_SMS_CMD) },
                { number, cmd->numberSize },
                { hc_STR_COMMA_LEN("\"\r") }
            };
            int64_t written = sys_writev(self->fd, &iov[0], hc_ARRAY_LEN(iov));
            if (written != (sizeof(modemClient_START_SMS_CMD) - 1) + cmd->numberSize + 2) return -1;
            break;
        }
        case modemClient_SEND_SMS: {
            struct modemClient_sendSms *cmd = (void *)&self->commandQueue[0];
            char *sms = (void *)&cmd[1];
            struct iovec_const iov[] = {
                { sms, cmd->smsSize },
                { hc_STR_COMMA_LEN("\x1a") }
            };
            int64_t written = sys_writev(self->fd, &iov[0], hc_ARRAY_LEN(iov));
            if (written != cmd->smsSize + 1) return -1;
            break;
        }
        default: hc_UNREACHABLE;
    }
    self->pendingCommand = true;
    struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
    CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
    return 0;
}

static int32_t modemClient_queueConstant(struct modemClient *self, const char *constant, uint16_t constantLength) {
    struct modemClient_constant *cmd;
    if (self->commandQueueSize > (int32_t)(sizeof(self->commandQueue) - sizeof(*cmd))) return -1;

    cmd = (void *)&self->commandQueue[self->commandQueueSize];
    cmd->tag = modemClient_CONSTANT;
    cmd->constantLength = constantLength;
    cmd->constant = constant;
    self->commandQueueSize += sizeof(*cmd);

    return modemClient_processQueue(self);
}

static int32_t modemClient_queueSms(struct modemClient *self, const char *number, uint8_t numberSize, const char *sms, uint16_t smsSize) {
    struct modemClient_startSms *startCmd;
    int32_t startCmdSize = (sizeof(*startCmd) + numberSize);
    struct modemClient_sendSms *sendCmd;
    int32_t sendCmdSize = (sizeof(*sendCmd) + smsSize);
    if (self->commandQueueSize > (int32_t)sizeof(self->commandQueue) - startCmdSize - sendCmdSize) return -1;

    startCmd = (void *)&self->commandQueue[self->commandQueueSize];
    startCmd->tag = modemClient_START_SMS;
    startCmd->numberSize = numberSize;
    hc_MEMCPY(&startCmd[1], number, numberSize);
    self->commandQueueSize += startCmdSize;

    sendCmd = (void *)&self->commandQueue[self->commandQueueSize];
    sendCmd->tag = modemClient_SEND_SMS;
    sendCmd->smsSize = smsSize;
    hc_MEMCPY(&sendCmd[1], sms, smsSize);
    self->commandQueueSize += sendCmdSize;

    return modemClient_processQueue(self);
}

static int32_t modemClient_ackPending(struct modemClient *self) {
    debug_ASSUME(self->pendingCommand);
    uint8_t tag = modemClient_pendingCmdTag(self);

    int32_t ackSize;
    switch (tag) {
        case modemClient_CONSTANT: ackSize = sizeof(struct modemClient_constant); break;
        case modemClient_START_SMS: {
            struct modemClient_startSms *cmd = (void *)&self->commandQueue[0];
            ackSize = sizeof(*cmd) + cmd->numberSize;
            break;
        }
        case modemClient_SEND_SMS: {
            struct modemClient_sendSms *cmd = (void *)&self->commandQueue[0];
            ackSize = sizeof(*cmd) + cmd->smsSize;
            break;
        }
        default: hc_UNREACHABLE;
    }
    debug_ASSERT(self->commandQueueSize >= ackSize);
    self->commandQueueSize -= ackSize;
    hc_MEMMOVE(&self->commandQueue[0], &self->commandQueue[ackSize], (uint64_t)self->commandQueueSize);
    self->pendingCommand = false;

    return modemClient_processQueue(self);
}

static void modemClient_onTimerFd(struct modemClient *self, int32_t epollFd) {
    if (self->fd < 0) { // Not connected, try to connect.
        self->fd = sys_openat(-1, &self->path[0], O_RDWR | O_NOCTTY, 0);
        debug_ASSERT(self->fd > 0 || self->fd == -ENOENT);
        if (self->fd < 0) goto out_fail;

        // Disable weird tty features, except ignoring CR. See termios(3).
        struct termios termios = {0};
        if (sys_ioctl(self->fd, TCGETS, &termios) != 0) goto out_fail;
        termios.c_iflag = IGNCR;
        termios.c_oflag &= ~(OPOST | OLCUC | ONLCR | OCRNL | ONLRET);
        termios.c_cflag &= ~(CSIZE | PARENB);
        termios.c_cflag |= CS8;
        termios.c_lflag &= ~(ISIG | ICANON | ECHO | IEXTEN);
        if (sys_ioctl(self->fd, TCSETS, &termios) != 0) goto out_fail;

        self->receivedSize = 0;
        self->commandQueueSize = 0;
        self->pendingCommand = false;
        if (modemClient_queueConstant(self, hc_STR_COMMA_LEN(modemClient_INIT_CMD)) < 0) goto out_fail;

        epollAdd(epollFd, self->fd);
    } else if (self->pendingCommand) goto out_fail; // Some command timed out.

    // Poll for SMS.
    if (modemClient_queueConstant(self, hc_STR_COMMA_LEN(modemClient_SMS_POLL_CMD)) < 0) goto out_fail;
    return;

    out_fail:
    modemClient_disconnect(self);
    struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
    CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
}

static void modemClient_onFd(struct modemClient *self) {
    int32_t lineStart = 0;

    int64_t read = sys_read(self->fd, &self->buffer[self->receivedSize], (int32_t)sizeof(self->buffer) - self->receivedSize);
    if (read <= 0) goto out_fail;
    self->receivedSize += read;

    uint8_t pendingCmdTag = modemClient_pendingCmdTag(self);
    int32_t lineLength;
    for (;; lineStart += lineLength + 1) {
        char *bufferLineStart = &self->buffer[lineStart];

        // Find line length, excluding new line.
        for (lineLength = 0; lineLength < self->receivedSize - lineStart; ++lineLength) {
            if (bufferLineStart[lineLength] == '\n') goto foundLine;
        }
        if (lineLength != 2 || bufferLineStart[0] != '>' || bufferLineStart[1] != ' ') goto out; // No line found.
        foundLine:;

        // OK or `> `.
        if (
            lineLength == 2 &&
            (
                (bufferLineStart[0] == 'O' && bufferLineStart[1] == 'K') ||
                (bufferLineStart[0] == '>' && bufferLineStart[1] == ' ')
            )
        ) {
            if (pendingCmdTag == modemClient_NO_CMD) goto out_fail;

            if (bufferLineStart[0] == '>') {
                if (pendingCmdTag != modemClient_START_SMS) goto out_fail;
                --lineLength; // `> ` doesn't have a newline, so compensate here.
            } else if (pendingCmdTag == modemClient_START_SMS) goto out_fail;

            if (modemClient_ackPending(self) < 0) goto out_fail;
            continue;
        }

        // Error.
        if (lineLength == 5 && mem_compare(&bufferLineStart[0], hc_STR_COMMA_LEN("ERROR")) == 0) {
            sys_write(2, hc_STR_COMMA_LEN("AT error\n"));
            goto out_fail;
        }

        // Read SMS.
        if (lineLength >= 7 && mem_compare(&bufferLineStart[0], hc_STR_COMMA_LEN("+CMGL: ")) == 0) {
            // Find end of slot number.
            int32_t slotNumberEnd = 7;
            for (; slotNumberEnd < lineLength; ++slotNumberEnd) {
                if (bufferLineStart[slotNumberEnd] == ',') goto foundSlotNumberEnd;
            }
            goto out_fail; // Not found.
            foundSlotNumberEnd:;

            // Find start of sender number.
            int32_t senderNumberIndex = slotNumberEnd + 15;

            // Find end of sender number.
            int32_t senderNumberEnd = senderNumberIndex;
            for (; senderNumberEnd < lineLength; ++senderNumberEnd) {
                if (bufferLineStart[senderNumberEnd] == '"') goto foundSenderNumberEnd;
            }
            goto out_fail; // Not found.
            foundSenderNumberEnd:;

            // Find SMS length (by locating last comma).
            int32_t smsLengthIndex = lineLength;
            for (;;) { // We know there is atleast one comma in the line, so skip checks.
                if (bufferLineStart[smsLengthIndex - 1] == ',') goto foundSmsLength;
                --smsLengthIndex;
            }
            goto out_fail; // Not found.
            foundSmsLength:;

            uint64_t smsLength;
            int32_t parsed = util_strToUint(&bufferLineStart[smsLengthIndex], lineLength - smsLengthIndex, &smsLength);
            if (parsed <= 0) goto out_fail;
            int32_t smsContentIndex = lineLength + 1;

            // Check if we have received the whole SMS and newline. Very ugly check..
            if (smsLength > (uint64_t)((self->receivedSize - smsContentIndex - 1) / 4)) goto out; // Nope.

            uint64_t smsSize = smsLength * 4; // UCS2 + hex encoding (ex: "ab" => "00610062").

            // Add the SMS size and newline to the "line" length.
            lineLength += smsSize + 1;

            // Check for `ip` command.
            if (smsLength != 2 || mem_compare(&bufferLineStart[smsContentIndex], hc_STR_COMMA_LEN("00690070")) != 0) continue;

            // Respond with DHCP IP.
            char *pos = util_intToStr(&buffer[4096], dhcpClient.leasedIpNetmask);
            *--pos = '/';
            pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[3]);
            *--pos = '.';
            pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[2]);
            *--pos = '.';
            pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[1]);
            *--pos = '.';
            pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[0]);
            int32_t sendSmsLength = (int32_t)(&buffer[4096] - pos);

            // Convert to UCS2 + hex encoding.
            uint16_t *ucs2Pos = (uint16_t *)hc_ASSUME_ALIGNED(pos - sendSmsLength, 2);
            util_strToUtf16(ucs2Pos, pos, sendSmsLength);
            char *ucs2HexPos = (char *)(ucs2Pos - sendSmsLength);
            for (uint16_t i = 0; i < sendSmsLength; ++i) {
                uint16_t ch = ucs2Pos[i];
                int32_t hexI = 4 * i;
                ucs2HexPos[hexI] = util_hexTable[ch >> 12];
                ucs2HexPos[hexI + 1] = util_hexTable[(ch >> 8) & 0xF];
                ucs2HexPos[hexI + 2] = util_hexTable[(ch >> 4) & 0xF];
                ucs2HexPos[hexI + 3] = util_hexTable[ch & 0xF];
            }

            if (
                modemClient_queueSms(
                    self,
                    &bufferLineStart[senderNumberIndex], (uint8_t)(senderNumberEnd - senderNumberIndex),
                    ucs2HexPos, (uint16_t)(4 * sendSmsLength)
                ) < 0
            ) goto out_fail;
            continue;
        }

        // SMS notification.
        if (lineLength >= 11 && mem_compare(&bufferLineStart[0], hc_STR_COMMA_LEN("+CMTI: \"SM\"")) == 0) {
            if (modemClient_queueConstant(self, hc_STR_COMMA_LEN(modemClient_SMS_POLL_CMD)) < 0) goto out_fail;
            continue;
        }
    }

    out_fail:
    modemClient_disconnect(self);
    out:
    if (modemClient_processQueue(self) < 0) goto out_fail;
    // Move incomplete line to start of buffer.
    self->receivedSize -= lineStart;
    hc_MEMMOVE(&self->buffer[0], &self->buffer[lineStart], (uint64_t)self->receivedSize);
}

static void modemClient_deinit(struct modemClient *self) {
    modemClient_disconnect(self);
    debug_CHECK(sys_close(self->timerFd), RES == 0);
}

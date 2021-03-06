struct x11Client {
    struct x11_setupResponse *setupResponse;
    int32_t setupResponseLength;
    int32_t socketFd;
    uint8_t receiveBuffer[4096];
    uint32_t receiveLength;
    uint32_t nextId;
    uint16_t sequenceNumber;
    uint8_t __pad[6];
};

static int32_t x11Client_init(struct x11Client *self, void *sockaddr, int32_t sockaddrLen, struct xauth_entry *authEntry) {
    self->sequenceNumber = 1;
    self->nextId = 0;
    self->receiveLength = 0;

    uint16_t sockaddrFamily = *(uint16_t *)sockaddr;
    self->socketFd = sys_socket(sockaddrFamily, SOCK_STREAM, 0);
    if (self->socketFd < 0) return -1;

    int32_t status = sys_connect(self->socketFd, sockaddr, sockaddrLen);
    if (status < 0) {
        status = -2;
        goto cleanup_socket;
    }
    struct x11_setupRequest request = {
        .byteOrder = x11_setupRequest_LITTLE_ENDIAN,
        .protocolMajorVersion = x11_setupRequest_PROTOCOL_MAJOR_VERSION,
        .protocolMinorVersion = x11_setupRequest_PROTOCOL_MINOR_VERSION,
        .authProtocolNameLength = authEntry->nameLength,
        .authProtocolDataLength = authEntry->dataLength
    };
    struct iovec iov[5] = {
        { .iov_base = &request,        .iov_len = sizeof(request) },
        { .iov_base = authEntry->name, .iov_len = request.authProtocolNameLength },
        { .iov_base = &request,        .iov_len = util_PAD_BYTES(request.authProtocolNameLength, 4) },
        { .iov_base = authEntry->data, .iov_len = request.authProtocolDataLength },
        { .iov_base = &request,        .iov_len = util_PAD_BYTES(request.authProtocolDataLength, 4) }
    };
    int64_t sendLength = sizeof(request) + util_ALIGN_FORWARD(request.authProtocolNameLength, 4) + util_ALIGN_FORWARD(request.authProtocolDataLength, 4);
    if (sys_sendmsg(self->socketFd, &(struct msghdr) { .msg_iov = &iov[0], .msg_iovlen = 5 }, MSG_NOSIGNAL) != sendLength) {
        status = -3;
        goto cleanup_socket;
    }

    struct x11_setupResponse response;
    int64_t numRead = sys_recvfrom(self->socketFd, &response, sizeof(response), 0, NULL, NULL);
    if (numRead <= 0) {
        status = -4;
        goto cleanup_socket;
    }

    // We have read at least 1 byte, and status is the first byte.
    if (response.status != x11_setupResponse_SUCCESS) {
        status = -5;
        goto cleanup_socket;
    }

    // Read all fixed size content.
    while (numRead < (int64_t)sizeof(response)) {
        char *readPos = &((char *)&response)[numRead];
        int64_t read = sys_recvfrom(self->socketFd, readPos, (int64_t)sizeof(response) - numRead, 0, NULL, NULL);
        if (read <= 0) {
            status = -6;
            goto cleanup_socket;
        }
        numRead += read;
    }

    // Do some sanity checks. TODO: what about imageByteOrder and bitmapFormatBitOrder?
    if (response.numRoots < 1) {
        status = -7;
        goto cleanup_socket;
    }

    // Allocate space for rest of response.
    self->setupResponseLength = 8 + (int32_t)response.length * 4;
    self->setupResponse = sys_mmap(NULL, self->setupResponseLength, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if ((int64_t)self->setupResponse < 0) {
        status = -7;
        goto cleanup_socket;
    }
    hc_MEMCPY(self->setupResponse, &response, sizeof(response));

    // Read rest of response.
    while (numRead < self->setupResponseLength) {
        char *readPos = &((char *)self->setupResponse)[numRead];
        int64_t read = sys_recvfrom(self->socketFd, readPos, self->setupResponseLength - numRead, 0, NULL, NULL);
        if (read <= 0) {
            status = -8;
            goto cleanup_setupResponse;
        }
        numRead += read;
    }
    return 0;

    cleanup_setupResponse:
    debug_CHECK(sys_munmap(self->setupResponse, self->setupResponseLength), RES == 0);
    cleanup_socket:
    debug_CHECK(sys_close(self->socketFd), RES == 0);
    return status;
}

static uint32_t x11Client_nextId(struct x11Client *self) {
    uint32_t nextId = self->setupResponse->resourceIdBase | self->nextId;
    uint32_t idMask = self->setupResponse->resourceIdMask;
    self->nextId += idMask & -idMask;
    return nextId;
}

// Returns sequence number of first request, or negative error code.
static int32_t x11Client_sendRequests(struct x11Client *self, void *requests, int64_t requestsLength, int32_t numRequests) {
    int64_t numSent = 0;
    do {
        int64_t sent = sys_sendto(self->socketFd, &((char *)requests)[numSent], requestsLength - numSent, MSG_NOSIGNAL, NULL, 0);
        if (sent <= 0) {
            if (sent == -EINTR) continue;
            return -1;
        }
        numSent += sent;
    } while (numSent < requestsLength);
    int32_t sequenceNumber = self->sequenceNumber;
    self->sequenceNumber += numRequests;
    return sequenceNumber;
}

static int32_t x11Client_receive(struct x11Client *self) {
    int32_t numRead = (int32_t)sys_recvfrom(
        self->socketFd,
        &self->receiveBuffer[self->receiveLength],
        sizeof(self->receiveBuffer) - self->receiveLength,
        0, NULL, NULL
    );
    if (numRead <= 0) return numRead;
    self->receiveLength += (uint32_t)numRead;
    return numRead;
}

// Returns length of next message, 0 if no messages are buffered.
static int32_t x11Client_nextMessage(struct x11Client *self) {
    if (self->receiveLength < 1) return 0;
    uint8_t typeMasked = self->receiveBuffer[0] & x11_TYPE_MASK;
    if (typeMasked != x11_TYPE_REPLY && typeMasked != x11_genericEvent_TYPE) {
        // Errors and standard events are all 32 bytes.
        return self->receiveLength >= 32 ? 32 : 0;
    }
    // 8 bytes must have been read to figure out reply or generic event length.
    if (self->receiveLength < 8) return 0;
    uint32_t length = 8 + *(uint32_t *)&self->receiveBuffer[4];
    return (self->receiveLength >> 2) >= length ? (int32_t)(length << 2) : 0;
}

// Acks a message of length `length`, so that the next one can be read.
static void x11Client_ackMessage(struct x11Client *self, int32_t length) {
    self->receiveLength -= (uint32_t)length;
    hc_MEMMOVE(&self->receiveBuffer[0], &self->receiveBuffer[length], self->receiveLength);
}

static void x11Client_deinit(struct x11Client *self) {
    debug_CHECK(sys_munmap(self->setupResponse, self->setupResponseLength), RES == 0);
    debug_CHECK(sys_close(self->socketFd), RES == 0);
}
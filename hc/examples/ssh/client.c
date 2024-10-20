#ifndef client_PAGE_SIZE
    #error "Please define `client_PAGE_SIZE`"
#endif

#define _client_MAX_PACKET_SIZE 35000 /* rfc4253, 6.1. */

#define _client_MAX_BLOCK_SIZE 64
#define _client_MAX_MAC_SIZE 16
#define _client_MIN_PADDING 4
#define _client_MAX_PADDING (_client_MIN_PADDING + 63)
#define _client_PADDING_SIZE(PAYLOAD_SIZE) (_client_MIN_PADDING + math_PAD_BYTES(4 + 1 + PAYLOAD_SIZE + _client_MIN_PADDING, _client_MAX_BLOCK_SIZE))

// Supported algorithms:
#define _client_KEX_CURVE25519 "curve25519-sha256"
#define _client_KEX_CURVE25519_LIBSSH "curve25519-sha256@libssh.org"
#define _client_KEX_LIST _client_KEX_CURVE25519 "," _client_KEX_CURVE25519_LIBSSH
#define _client_HOST_KEY_ED25519 "ssh-ed25519"
#define _client_HOST_KEY_LIST _client_HOST_KEY_ED25519
#define _client_CIPHER_CHACHA20POLY1305 "chacha20-poly1305@openssh.com"
#define _client_CIPHER_LIST _client_CIPHER_CHACHA20POLY1305

enum _client_encryption { _client_encryption_NONE, _client_encryption_CHACHA20POLY1305 };

struct client_messageHead {
    uint32_t _size;
    uint8_t _paddingSize;
} hc_PACKED(1);

struct client_messageTail {
    uint8_t _padding[_client_MAX_PADDING];
    uint8_t _mac[_client_MAX_MAC_SIZE];
};

struct client {
    uint8_t *buffer;
    int32_t bufferSize;
    int32_t bufferPos;
    int32_t receivedSize;
    int32_t bufferMemFd;

    uint32_t receiveSequenceNumber;
    uint32_t sendSequenceNumber;

    union chacha20 ciphers[4]; // enc aead, enc len, dec aead, dec len.

    struct sha256 partialExchangeHash;
    struct sha256 tempHash;
    uint8_t sessionId[sha256_HASH_SIZE];

    int32_t socketFd;
    enum _client_encryption encryption;

    int32_t nextSize; // Negated if already decrypted.
    int32_t __pad;
};

static const int32_t _client_macSize[] = {
    [_client_encryption_NONE] = 0,
    [_client_encryption_CHACHA20POLY1305] = poly1305_MAC_SIZE
};

static int32_t client_receive(struct client *self) {
    int32_t numRead = (int32_t)sys_recvfrom(
        self->socketFd,
        &self->buffer[self->bufferPos + self->receivedSize],
        self->bufferSize - self->receivedSize,
        0, NULL, NULL
    );
    self->receivedSize += numRead;
    return numRead;
}

// Returns size of message, or 0 if no message is buffered.
static int32_t client_peekMessage(struct client *self, uint8_t **message) {
    struct packet {
        uint32_t size;
        uint8_t paddingSize;
        uint8_t payload[3];
    } *packet = (struct packet *)hc_ASSUME_ALIGNED(&self->buffer[self->bufferPos], 4);

    if (self->nextSize < 0) {
        *message = &packet->payload[0];
        return -self->nextSize - packet->paddingSize - (int32_t)sizeof(packet->paddingSize);
    }

    int32_t macSize = _client_macSize[self->encryption];
    if (self->nextSize == 0) {
        if (self->receivedSize < (int32_t)sizeof(packet->size)) return 0;
        uint32_t size;
        switch (self->encryption) {
            case _client_encryption_NONE: {
                size = hc_BSWAP32(packet->size);
                break;
            }
            case _client_encryption_CHACHA20POLY1305: {
                mem_storeU64BE(&self->ciphers[3].orig.nonce, self->receiveSequenceNumber);
                union chacha20 stream;
                chacha20_block(&self->ciphers[3], &stream);
                size = hc_BSWAP32(packet->size ^ stream.u32[0]);
                break;
            }
            default: hc_UNREACHABLE;
        }
        if (size > _client_MAX_PACKET_SIZE - (uint32_t)sizeof(packet->size) - (uint32_t)macSize) return -1;
        if (size & 0x3) return -2; // rfc4253, 6. ensures max(8,blocksize) alignment, but we only need 4.
        self->nextSize = (int32_t)size;
    }

    int32_t packetSize = (int32_t)sizeof(packet->size) + self->nextSize;
    if (self->receivedSize < packetSize + macSize) return 0;

    switch (self->encryption) {
        case _client_encryption_NONE: break;
        case _client_encryption_CHACHA20POLY1305: {
            mem_storeU64BE(&self->ciphers[2].orig.nonce, self->receiveSequenceNumber);

            // Verify MAC.
            struct poly1305 poly1305;
            uint8_t calculatedMac[poly1305_MAC_SIZE];
            self->ciphers[2].orig.blockCounter = 0;
            union chacha20 stream;
            chacha20_block(&self->ciphers[2], &stream);
            poly1305_init(&poly1305, &stream.u8[0]);
            poly1305_update(&poly1305, packet, packetSize);
            poly1305_finish(&poly1305, &calculatedMac[0]);
            if (mem_compareConstantTime(&calculatedMac[0], (uint8_t *)packet + packetSize, poly1305_MAC_SIZE) != 0) return -3;

            // Decrypt packet (except size).
            for (int32_t offset = 4;;) {
                ++self->ciphers[2].orig.blockCounter;
                chacha20_block(&self->ciphers[2], &stream);

                int32_t iEnd;
                int32_t remaining = packetSize - offset;
                if (remaining < (int32_t)sizeof(stream)) iEnd = remaining;
                else iEnd = (int32_t)sizeof(stream);

                uint8_t *bufferPos = (uint8_t *)packet + offset;
                offset += iEnd;
                for (int32_t i = 0; i < iEnd; i += 4) {
                    *(uint32_t *)hc_ASSUME_ALIGNED(bufferPos + i, 4) ^= *(uint32_t *)hc_ASSUME_ALIGNED(&stream.u8[i], 4);
                }
                if (offset == packetSize) break;
            }
            break;
        }
        default: hc_UNREACHABLE;
    }

    int32_t messageSize = self->nextSize - packet->paddingSize - (int32_t)sizeof(packet->paddingSize);
    if (messageSize <= 0) return -4;

    ++self->receiveSequenceNumber;
    self->nextSize = -self->nextSize;

    *message = &packet->payload[0];
    return messageSize;
}

// Acknowledge peeked message, moving past it.
static void client_ackMessage(struct client *self) {
    debug_ASSERT(self->nextSize < 0);
    int32_t totalSize = -self->nextSize + (int32_t)sizeof(uint32_t) + _client_macSize[self->encryption];
    self->bufferPos = (self->bufferPos + totalSize) % self->bufferSize;
    self->receivedSize -= totalSize;
    self->nextSize = 0;
}

// Get the next message (peek and acknowledge combined).
static int32_t client_nextMessage(struct client *self, uint8_t **message) {
    int32_t messageSize = client_peekMessage(self, message);
    if (messageSize > 0) client_ackMessage(self);
    return messageSize;
}

static int32_t client_waitForMessage(struct client *self, uint8_t **message) {
    for (;;) {
        int32_t size = client_nextMessage(self, message);
        if (size > 0) return size;
        if (size < 0) {
            debug_printNum("Error waiting for message (", size, ")\n");
            return -1;
        }
        size = client_receive(self);
        if (size <= 0) return size;
    }
}

int32_t client_sendMessage(struct client *self, struct client_messageHead *packetHead, int32_t messageSize) {
    debug_ASSERT(messageSize > 0);

    int32_t paddingSize = _client_PADDING_SIZE(messageSize);
    int32_t packetSize = (int32_t)sizeof(*packetHead) + messageSize + paddingSize;
    packetHead->_size = hc_BSWAP32((uint32_t)packetSize - 4);
    packetHead->_paddingSize = (uint8_t)paddingSize;

    switch (self->encryption) {
        case _client_encryption_NONE: break;
        case _client_encryption_CHACHA20POLY1305: {
            // Encrypt size.
            mem_storeU64BE(&self->ciphers[1].orig.nonce, self->sendSequenceNumber);
            union chacha20 stream;
            chacha20_block(&self->ciphers[1], &stream);
            packetHead->_size ^= stream.u32[0];

            // Encrypt packet (except size).
            mem_storeU64BE(&self->ciphers[0].orig.nonce, self->sendSequenceNumber);
            self->ciphers[0].orig.blockCounter = 0;
            for (int32_t offset = 4;;) {
                ++self->ciphers[0].orig.blockCounter;
                chacha20_block(&self->ciphers[0], &stream);

                int32_t iEnd;
                int32_t remaining = packetSize - offset;
                if (remaining < (int32_t)sizeof(stream)) iEnd = remaining;
                else iEnd = (int32_t)sizeof(stream);

                uint8_t *bufferPos = (uint8_t *)packetHead + offset;
                offset += iEnd;
                for (int32_t i = 0; i < iEnd; i += 4) {
                    *(uint32_t *)hc_ASSUME_ALIGNED(bufferPos + i, 4) ^= *(uint32_t *)hc_ASSUME_ALIGNED(&stream.u8[i], 4);
                }
                if (offset == packetSize) break;
            }

            // Append MAC.
            struct poly1305 poly1305;
            self->ciphers[0].orig.blockCounter = 0;
            chacha20_block(&self->ciphers[0], &stream);
            poly1305_init(&poly1305, &stream.u8[0]);
            poly1305_update(&poly1305, packetHead, packetSize);
            poly1305_finish(&poly1305, (uint8_t *)packetHead + packetSize);
            packetSize += poly1305_MAC_SIZE;
            break;
        }
    }

    int32_t numSent = (int32_t)sys_sendto(self->socketFd, packetHead, packetSize, MSG_NOSIGNAL, NULL, 0);
    if (numSent != packetSize) return -1;
    ++self->sendSequenceNumber;
    return 0;
}

static int32_t _client_doHello(struct client *self) {
    #define _client_HELLO_IDENTIFICATION "SSH-2.0-hc"
    static const struct {
        uint32_t identificationSize;
        char identificationLine[hc_STR_LEN(_client_HELLO_IDENTIFICATION) + 2];
    } hello = {
        .identificationSize = hc_BSWAP32(hc_STR_LEN(_client_HELLO_IDENTIFICATION)),
        .identificationLine = _client_HELLO_IDENTIFICATION "\r\n"
    };
    int64_t numSent = sys_sendto(self->socketFd, &hello.identificationLine, sizeof(hello.identificationLine), MSG_NOSIGNAL, NULL, 0);
    if (numSent != sizeof(hello.identificationLine)) return -1;

    sha256_init(&self->partialExchangeHash);
    sha256_update(&self->partialExchangeHash, &hello, sizeof(hello) - 2);

    // Not implemented: The server may send other lines of data before the version string. rfc4253, 4.2.
    int32_t received = 0;
    for (int32_t pos = 0;;) {
        int32_t numRead = (int32_t)sys_recvfrom(
            self->socketFd,
            &self->buffer[pos],
            self->bufferSize - received,
            0, NULL, NULL
        );
        if (numRead <= 0) return -2;
        received += numRead;

        int32_t end = received;
        if (end > 255) end = 255; // rfc4253, 4.2.

        for (; pos + 1 < end; ++pos) {
            if (self->buffer[pos] == '\r') {
                if (self->buffer[pos + 1] != '\n') return -3;

                if (pos < (int64_t)hc_STR_LEN("SSH-2.0-") || mem_compare(&self->buffer[0], "SSH-2.0-", hc_STR_LEN("SSH-2.0-")) != 0) return -4;
                sha256_update(&self->partialExchangeHash, &(uint32_t) { hc_BSWAP32((uint32_t)pos) }, 4);
                sha256_update(&self->partialExchangeHash, &self->buffer[0], pos);

                int32_t bufferPos = pos + 2;
                self->receivedSize = received - bufferPos;

                // Align buffer position to 4 bytes.
                self->bufferPos = math_ALIGN_FORWARD(bufferPos, 4);
                if (self->bufferPos != bufferPos) hc_MEMMOVE(&self->buffer[self->bufferPos], &self->buffer[bufferPos], (size_t)self->receivedSize);
                return 0;
            }
        }
    }
}

static int32_t _client_sendKeyExchangeInit(struct client *self) {
    #define _client_MAC_LIST ""
    #define _client_COMPRESSION_LIST "none"
    #define _client_LANGUAGE_LIST ""
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint8_t cookie[16];
            uint32_t kexListSize;
            char kexList[hc_STR_LEN(_client_KEX_LIST)];
            uint32_t hostKeyListSize;
            char hostKeyList[hc_STR_LEN(_client_HOST_KEY_LIST)];
            uint32_t clientCipherListSize;
            char clientCipherList[hc_STR_LEN(_client_CIPHER_LIST)];
            uint32_t serverCipherListSize;
            char serverCipherList[hc_STR_LEN(_client_CIPHER_LIST)];
            uint32_t clientMacListSize;
            char clientMacList[hc_STR_LEN(_client_MAC_LIST)];
            uint32_t serverMacListSize;
            char serverMacList[hc_STR_LEN(_client_MAC_LIST)];
            uint32_t clientCompressionListSize;
            char clientCompressionList[hc_STR_LEN(_client_COMPRESSION_LIST)];
            uint32_t serverCompressionListSize;
            char serverCompressionList[hc_STR_LEN(_client_COMPRESSION_LIST)];
            uint32_t clientLanguageListSize;
            char clientLanguageList[hc_STR_LEN(_client_LANGUAGE_LIST)];
            uint32_t serverLanguageListSize;
            char serverLanguageList[hc_STR_LEN(_client_LANGUAGE_LIST)];
            uint8_t firstKexPacketFollows;
            uint32_t reserved;
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } keyExchangeInit = {
        .message = {
            .opcode = proto_MSG_KEXINIT,
            .kexListSize = hc_BSWAP32(hc_STR_LEN(_client_KEX_LIST)),
            .kexList = _client_KEX_LIST,
            .hostKeyListSize = hc_BSWAP32(hc_STR_LEN(_client_HOST_KEY_LIST)),
            .hostKeyList = _client_HOST_KEY_LIST,
            .clientCipherListSize = hc_BSWAP32(hc_STR_LEN(_client_CIPHER_LIST)),
            .clientCipherList = _client_CIPHER_LIST,
            .serverCipherListSize = hc_BSWAP32(hc_STR_LEN(_client_CIPHER_LIST)),
            .serverCipherList = _client_CIPHER_LIST,
            .clientMacListSize = hc_BSWAP32(hc_STR_LEN(_client_MAC_LIST)),
            .clientMacList = _client_MAC_LIST,
            .serverMacListSize = hc_BSWAP32(hc_STR_LEN(_client_MAC_LIST)),
            .serverMacList = _client_MAC_LIST,
            .clientCompressionListSize = hc_BSWAP32(hc_STR_LEN(_client_COMPRESSION_LIST)),
            .clientCompressionList = _client_COMPRESSION_LIST,
            .serverCompressionListSize = hc_BSWAP32(hc_STR_LEN(_client_COMPRESSION_LIST)),
            .serverCompressionList = _client_COMPRESSION_LIST,
            .clientLanguageListSize = hc_BSWAP32(hc_STR_LEN(_client_LANGUAGE_LIST)),
            .clientLanguageList = _client_LANGUAGE_LIST,
            .serverLanguageListSize = hc_BSWAP32(hc_STR_LEN(_client_LANGUAGE_LIST)),
            .serverLanguageList = _client_LANGUAGE_LIST,
            .firstKexPacketFollows = 0,
            .reserved = 0
        }
    };

    if (sys_getrandom(&keyExchangeInit.message.cookie, sizeof(keyExchangeInit.message.cookie), 0) < 0) return -1;

    self->tempHash = self->partialExchangeHash;
    sha256_update(&self->tempHash, &(uint32_t) { hc_BSWAP32(sizeof(keyExchangeInit.message)) }, 4);
    sha256_update(&self->tempHash, &keyExchangeInit.message, sizeof(keyExchangeInit.message));

    int32_t status = client_sendMessage(self, &keyExchangeInit.head, sizeof(keyExchangeInit.message));
    if (status < 0) return -2;
    return 0;
}

static bool _client_nameListContains(void *nameList, uint32_t nameListSize, void *string, uint32_t stringSize) {
    for (uint32_t i = 0;;) {
        if ((nameListSize - i) < stringSize) return false;
        if (mem_compare(nameList + i, string, stringSize) == 0) return true;
        for (; i < nameListSize; ++i) {
            if (*(char *)(nameList + i) == ',') {
                ++i;
                break;
            }
        }
    }
}

// Converts 32 byte shared secret to mpint (rfc4251, 5.), and updates hash with it.
static void _client_sha256UpdateSharedSecret(struct sha256 *sha256, uint8_t *secret) {
    int32_t pos = 0;
    for (; pos < 32; ++pos) if (secret[pos] != 0) break;
    int32_t extraZero = secret[pos] >> 7;
    sha256_update(sha256, &(uint32_t) { hc_BSWAP32((uint32_t)(32 - pos + extraZero)) }, 4);
    if (extraZero) sha256_update(sha256, &(uint8_t) { 0 }, 1);
    sha256_update(sha256, secret + pos, 32 - pos);
}

static int32_t _client_doKeyExchange(struct client *self, void *serverInit, int32_t serverInitSize) {
    // Parse server KEXINIT message.
    if (serverInitSize < 1 + 16 + 4) return -1; // Check size includes kexListSize.

    uint8_t *serverInitCurr = serverInit;
    if (*serverInitCurr != proto_MSG_KEXINIT) return -2;

    uint8_t *serverInitEnd = serverInitCurr + serverInitSize;

    serverInitCurr += 1 + 16; // Jump to kexListSize.
    uint32_t kexListSize = mem_loadU32BE(serverInitCurr);
    serverInitCurr += 4; // Jump to kexList.
    if (serverInitCurr + kexListSize + 4 > serverInitEnd) return -3; // Check size includes hostKeyListSize.
    if (
        !_client_nameListContains(serverInitCurr, kexListSize, hc_STR_COMMA_LEN(_client_KEX_CURVE25519)) &&
        !_client_nameListContains(serverInitCurr, kexListSize, hc_STR_COMMA_LEN(_client_KEX_CURVE25519_LIBSSH))
    ) return -4;
    serverInitCurr += kexListSize; // Jump to hostKeyListSize.

    uint32_t hostKeyListSize = mem_loadU32BE(serverInitCurr);
    serverInitCurr += 4; // Jump to hostKeyList.
    if (serverInitCurr + hostKeyListSize + 4 > serverInitEnd) return -5; // Check size includes clientCipherListSize.
    if (!_client_nameListContains(serverInitCurr, hostKeyListSize, hc_STR_COMMA_LEN(_client_HOST_KEY_ED25519))) return -6;
    serverInitCurr += hostKeyListSize; // Jump to clientCipherListSize.

    for (int32_t i = 0; i < 2; ++i) {
        uint32_t cipherListSize = mem_loadU32BE(serverInitCurr);
        serverInitCurr += 4; // Jump to clientCipherList (i=0), serverCipherList (i=1).
        if (serverInitCurr + cipherListSize + 4 > serverInitEnd) return -7; // Check size includes serverCipherListSize (i=0), clientMacListSize (i=1).
        if (!_client_nameListContains(serverInitCurr, cipherListSize, hc_STR_COMMA_LEN(_client_CIPHER_CHACHA20POLY1305))) return -8;
        serverInitCurr += cipherListSize; // Jump to serverCipherListSize (i=0), clientMacListSize (i=1).
    }

    // Skip the remaining 6 lists.
    for (int32_t i = 0; i < 6; ++i) {
        if (serverInitCurr + 4 > serverInitEnd) return -9; // Check size includes listSize.
        uint32_t listSize = mem_loadU32BE(serverInitCurr);
        serverInitCurr += 4 + listSize; // Jump to next listSize (i<5), firstKexPacketFollows (i=5).
    }
    if (serverInitCurr + 1 + 4 != serverInitEnd) return -10; // Check size exactly includes firstKexPacketFollows and reserved.

    bool firstKexPacketFollows = *serverInitCurr != 0;
    if (firstKexPacketFollows) return -11; // TODO: Is this valid for a server to send?

    sha256_update(&self->tempHash, &(uint32_t) { hc_BSWAP32((uint32_t)serverInitSize) }, 4);
    sha256_update(&self->tempHash, serverInit, serverInitSize);

    // Send KEX_ECDH_INIT message.
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t publicKeySize;
            uint8_t publicKey[32];
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } ecdhInit = {
        .message = {
            .opcode = proto_MSG_KEX_ECDH_INIT,
            .publicKeySize = hc_BSWAP32(sizeof(ecdhInit.message.publicKey))
        }
    };
    uint8_t publicKey[32];
    uint8_t secret[32];
    if (sys_getrandom(&secret[0], sizeof(secret), 0) < 0) return -12;
    x25519(&publicKey[0], &secret[0], &x25519_ecdhBasepoint[0]);
    hc_MEMCPY(&ecdhInit.message.publicKey[0], &publicKey[0], sizeof(publicKey));

    int32_t status = client_sendMessage(self, &ecdhInit.head, sizeof(ecdhInit.message));
    if (status < 0) return -13;

    // Wait for server's KEX_ECDH_REPLY.
    struct {
        uint8_t opcode;
        uint32_t hostKeySize;
        struct {
            uint32_t keyTypeSize;
            uint8_t keyType[hc_STR_LEN(_client_HOST_KEY_ED25519)];
            uint32_t publicKeySize;
            uint8_t publicKey[32];
        } hc_PACKED(1) hostKey;
        uint32_t publicKeySize;
        uint8_t publicKey[32];
        uint32_t fullSignatureSize;
        struct {
            uint32_t signatureTypeSize;
            uint8_t signatureType[hc_STR_LEN(_client_HOST_KEY_ED25519)];
            uint32_t signatureSize;
            uint8_t signature[ed25519_SIGNATURE_SIZE];
        } hc_PACKED(1) fullSignature;
    } hc_PACKED(1) *ecdhReply;
    int32_t ecdhReplySize = client_waitForMessage(self, (uint8_t **)&ecdhReply);
    if (ecdhReplySize != sizeof(*ecdhReply)) return -14;
    if (
        ecdhReply->opcode != proto_MSG_KEX_ECDH_REPLY ||
        ecdhReply->hostKeySize != hc_BSWAP32(sizeof(ecdhReply->hostKey)) ||
        ecdhReply->hostKey.keyTypeSize != hc_BSWAP32(sizeof(ecdhReply->hostKey.keyType)) ||
        mem_compare(ecdhReply->hostKey.keyType, _client_HOST_KEY_ED25519, sizeof(ecdhReply->hostKey.keyType)) != 0 ||
        ecdhReply->hostKey.publicKeySize != hc_BSWAP32(32) ||
        ecdhReply->publicKeySize != hc_BSWAP32(32) ||
        ecdhReply->fullSignatureSize != hc_BSWAP32(sizeof(ecdhReply->fullSignature)) ||
        ecdhReply->fullSignature.signatureTypeSize != hc_BSWAP32(sizeof(ecdhReply->fullSignature.signatureType)) ||
        mem_compare(ecdhReply->fullSignature.signatureType, _client_HOST_KEY_ED25519, sizeof(ecdhReply->fullSignature.signatureType)) != 0 ||
        ecdhReply->fullSignature.signatureSize != hc_BSWAP32(sizeof(ecdhReply->fullSignature.signature))
    ) return -15;

    // Calculate shared secret.
    uint8_t sharedSecret[32];
    x25519(&sharedSecret[0], &secret[0], &ecdhReply->publicKey[0]);

    // Finish exchange hash.
    uint8_t exchangeHash[sha256_HASH_SIZE];
    sha256_update(&self->tempHash, &ecdhReply->hostKeySize, sizeof(ecdhReply->hostKeySize) + sizeof(ecdhReply->hostKey));
    sha256_update(&self->tempHash, &(uint32_t) { hc_BSWAP32(sizeof(publicKey)) }, 4);
    sha256_update(&self->tempHash, &publicKey[0], sizeof(publicKey));
    sha256_update(&self->tempHash, &ecdhReply->publicKeySize, sizeof(ecdhReply->publicKeySize) + sizeof(ecdhReply->publicKey));
    _client_sha256UpdateSharedSecret(&self->tempHash, &sharedSecret[0]);
    sha256_finish(&self->tempHash, &exchangeHash[0]);

    // Verify signature.
    status = ed25519_verify(&exchangeHash[0], sha256_HASH_SIZE, &ecdhReply->hostKey.publicKey[0], &ecdhReply->fullSignature.signature[0]);
    if (status != 0) return -16;

    // Set session id.
    if (self->encryption == _client_encryption_NONE) hc_MEMCPY(&self->sessionId[0], &exchangeHash[0], sizeof(self->sessionId));

    // Send NEWKEYS message.
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
        } message;
        struct client_messageTail tail;
    } newKeys = {
        .message = {
            .opcode = proto_MSG_NEWKEYS
        }
    };
    status = client_sendMessage(self, &newKeys.head, sizeof(newKeys.message));
    if (status < 0) return -17;

    // Wait for server's NEWKEYS message.
    uint8_t *serverNewKeys;
    int32_t serverNewKeysSize = client_waitForMessage(self, &serverNewKeys);
    if (serverNewKeysSize != 1) return -18;
    if (*serverNewKeys != proto_MSG_NEWKEYS) return -19;

    self->encryption = _client_encryption_CHACHA20POLY1305;

    // Derive encryption and decryption keys.
    for (int32_t i = 0; i < 2; ++i) {
        sha256_init(&self->tempHash);
        _client_sha256UpdateSharedSecret(&self->tempHash, &sharedSecret[0]);
        sha256_update(&self->tempHash, &exchangeHash[0], sizeof(exchangeHash));
        sha256_update(&self->tempHash, &(uint8_t) { (uint8_t)('C' + i) }, 1);
        sha256_update(&self->tempHash, &self->sessionId[0], sizeof(self->sessionId));
        sha256_finish(&self->tempHash, &self->ciphers[2 * i].orig.key);

        // Extend once.
        sha256_init(&self->tempHash);
        _client_sha256UpdateSharedSecret(&self->tempHash, &sharedSecret[0]);
        sha256_update(&self->tempHash, &exchangeHash[0], sizeof(exchangeHash));
        sha256_update(&self->tempHash, &self->ciphers[2 * i].orig.key, 32);
        sha256_finish(&self->tempHash, &self->ciphers[2 * i + 1].orig.key);
    }
    return 0;
}

static int32_t _client_doServiceRequest(struct client *self) {
    #define _client_SERVICE_NAME "ssh-userauth"
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t serviceNameSize;
            char serviceName[hc_STR_LEN(_client_SERVICE_NAME)];
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } serviceRequest = {
        .message = {
            .opcode = proto_MSG_SERVICE_REQUEST,
            .serviceNameSize = hc_BSWAP32(hc_STR_LEN(_client_SERVICE_NAME)),
            .serviceName = _client_SERVICE_NAME
        }
    };
    int32_t status = client_sendMessage(self, &serviceRequest.head, sizeof(serviceRequest.message));
    if (status < 0) return -1;

    struct {
        uint8_t opcode;
        uint32_t serviceNameSize;
        char serviceName[hc_STR_LEN(_client_SERVICE_NAME)];
    } hc_PACKED(1) *response;
    int32_t responseSize = client_waitForMessage(self, (uint8_t **)&response);
    if (responseSize != sizeof(*response)) return -2;

    if (
        response->opcode != proto_MSG_SERVICE_ACCEPT ||
        response->serviceNameSize != hc_BSWAP32(hc_STR_LEN(_client_SERVICE_NAME)) ||
        mem_compare(&response->serviceName[0], _client_SERVICE_NAME, hc_STR_LEN(_client_SERVICE_NAME)) != 0
    ) return -3;
    return 0;
}

static int32_t client_connect(struct client *self, void *sockaddr, int32_t sockaddrSize) {
    self->bufferPos = 0;
    self->receivedSize = 0;
    self->receiveSequenceNumber = 0;
    self->sendSequenceNumber = 0;
    self->encryption = _client_encryption_NONE;
    self->nextSize = 0;

    // Connect to server.
    int32_t status = sys_connect(self->socketFd, sockaddr, sockaddrSize);
    if (status < 0) return -1;

    status = _client_doHello(self);
    if (status < 0) return -2;

    status = _client_sendKeyExchangeInit(self);
    if (status < 0) return -3;

    // Receive next message (should be a KEXINIT).
    uint8_t *serverMessage;
    int32_t serverMessageSize = client_waitForMessage(self, &serverMessage);
    if (serverMessageSize <= 0) return -4;

    // Perform key exchange.
    status = _client_doKeyExchange(self, serverMessage, serverMessageSize);
    if (status < 0) {
        debug_printNum("Key exchange failed (", status, ")\n");
        return -5;
    }

    status = _client_doServiceRequest(self);
    if (status < 0) return -6;
    return 0;
}

static int32_t client_init(struct client *self, int32_t sockaddrFamily) {
    for (int32_t i = 0; i < (int32_t)hc_ARRAY_LEN(self->ciphers); ++i) {
        chacha20_init(&self->ciphers[i]);
        // Skip setting nonce since it's always set before encryption/decryption.
        self->ciphers[i].orig.blockCounter = 0;
    }

    // Create circular buffer.
    self->bufferMemFd = sys_memfd_create("", MFD_CLOEXEC);
    if (self->bufferMemFd < 0) return -1;

    self->bufferSize = math_ALIGN_FORWARD(_client_MAX_PACKET_SIZE, (int32_t)client_PAGE_SIZE);
    int32_t status = sys_ftruncate(self->bufferMemFd, self->bufferSize);
    if (status < 0) {
        status = -2;
        goto cleanup_bufferMemFd;
    }

    self->buffer = sys_mmap(NULL, 2 * self->bufferSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ((int64_t)self->buffer < 0) {
        status = -3;
        goto cleanup_bufferMemFd;
    }

    for (int32_t i = 0; i < 2; ++i) {
        void *address = sys_mmap(
            self->buffer + i * self->bufferSize,
            self->bufferSize,
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED,
            self->bufferMemFd,
            0
        );
        if ((int64_t)address < 0) {
            status = -4;
            goto cleanup_buffer;
        }
    }

    self->socketFd = sys_socket(sockaddrFamily, SOCK_STREAM, 0);
    if (self->socketFd < 0) {
        status = -5;
        goto cleanup_buffer;
    }
    return 0;

    cleanup_buffer:
    debug_CHECK(sys_munmap(self->buffer, 2 * self->bufferSize), RES == 0);
    cleanup_bufferMemFd:
    debug_CHECK(sys_close(self->bufferMemFd), RES == 0);
    return status;
}

static void client_deinit(struct client *self) {
    debug_CHECK(sys_close(self->socketFd), RES == 0);
    debug_CHECK(sys_munmap(self->buffer, 2 * self->bufferSize), RES == 0);
    debug_CHECK(sys_close(self->bufferMemFd), RES == 0);
}

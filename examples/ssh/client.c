#ifndef client_PAGE_SIZE
    #error "Please define `client_PAGE_SIZE`"
#endif

#define _client_MAX_BLOCK_SIZE 64
#define _client_MIN_PADDING 4
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

struct client {
    uint8_t *buffer;
    int32_t bufferSize;
    int32_t bufferPos;
    int32_t receivedSize;
    int32_t bufferMemFd;

    uint32_t receiveSequenceNumber;
    uint32_t sendSequenceNumber; // TODO: Actually update and use this.

    union chacha20 ciphers[4]; // enc aead, enc len, dec aead, dec len.

    struct sha256 partialExchangeHash;
    struct sha256 tempHash;
    uint8_t sessionId[sha256_HASH_SIZE];

    int32_t socketFd;
    enum _client_encryption encryption;
};

static int32_t _client_receive(struct client *self) {
    int32_t numRead = (int32_t)sys_recvfrom(
        self->socketFd,
        &self->buffer[self->bufferPos + self->receivedSize],
        self->bufferSize - self->receivedSize,
        0, NULL, NULL
    );
    self->receivedSize += numRead;
    return numRead;
}

static int32_t _client_nextPacket(struct client *self, uint8_t **payload) {
    struct packet {
        uint32_t size;
        uint8_t paddingSize;
        uint8_t payload[];
    } hc_PACKED(1);
    struct packet *packet = (struct packet *)&self->buffer[self->bufferPos];

    if (self->receivedSize < 16) return 0; // rfc4253, 6.

    int64_t packetSize = 4;
    int32_t macSize = 0;
    switch (self->encryption) {
        case _client_encryption_NONE: {
            packetSize += hc_BSWAP32(packet->size);
            break;
        }
        case _client_encryption_CHACHA20POLY1305: {
            macSize = poly1305_MAC_SIZE;
            // Decrypt length.
            mem_storeU64BE(&self->ciphers[3].orig.nonce, self->receiveSequenceNumber);
            union chacha20 stream;
            chacha20_block(&self->ciphers[3], &stream);
            packetSize += hc_BSWAP32(packet->size ^ stream.u32[0]);
            break;
        }
        default: hc_UNREACHABLE;
    }

    if (packetSize & 0x3) return -1; // rfc4253, 6. ensures max(8,blocksize) alignment, but we only need 4.
    if (packetSize + macSize > self->bufferSize) return -2;
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
            if (mem_compareConstantTime(&calculatedMac[0], &self->buffer[self->bufferPos + (int32_t)packetSize], poly1305_MAC_SIZE) != 0) return -3;

            // Decrypt packet (except size).
            for (int64_t offset = 4;;) {
                ++self->ciphers[2].orig.blockCounter;
                chacha20_block(&self->ciphers[2], &stream);

                int64_t i = packetSize - offset;
                if (i > (int64_t)sizeof(stream)) i = sizeof(stream);
                uint8_t *bufferPos = &self->buffer[self->bufferPos + offset];
                offset += i;
                do {
                    i -= 4;
                    uint32_t temp = mem_loadU32(bufferPos + i);
                    temp ^= *(uint32_t *)hc_ASSUME_ALIGNED(&stream.u8[i], 4);
                    mem_storeU32(bufferPos + i, temp);
                } while (i != 0);
                if (offset == packetSize) break;
            }
            break;
        }
        default: hc_UNREACHABLE;
    }

    int32_t payloadSize = (int32_t)packetSize - packet->paddingSize - 4 - 1;
    if (payloadSize <= 0) return -4;

    ++self->receiveSequenceNumber;

    self->bufferPos = (self->bufferPos + (int32_t)packetSize + macSize) % self->bufferSize;
    self->receivedSize -= (int32_t)packetSize + macSize;
    *payload = &packet->payload[0];
    return payloadSize;
}

static int32_t _client_waitForPacket(struct client *self, uint8_t **payload) {
    for (;;) {
        int32_t size = _client_nextPacket(self, payload);
        if (size > 0) return size;
        if (size < 0) {
            debug_printNum("Error waiting for packet (", size, ")\n");
            return -1;
        }
        size = _client_receive(self);
        if (size <= 0) return size;
    }
}

static int32_t _client_doHello(struct client *self) {
    #define _client_HELLO_IDENTIFICATION "SSH-2.0-hc"
    static const struct {
        uint32_t identificationSize;
        char identificationLine[(sizeof(_client_HELLO_IDENTIFICATION) - 1) + 2];
    } hello = {
        .identificationSize = hc_BSWAP32(sizeof(_client_HELLO_IDENTIFICATION) - 1),
        .identificationLine = _client_HELLO_IDENTIFICATION "\r\n"
    };
    int64_t numSent = sys_sendto(self->socketFd, &hello.identificationLine, sizeof(hello.identificationLine), MSG_NOSIGNAL, NULL, 0);
    if (numSent != sizeof(hello.identificationLine)) return -1;

    sha256_init(&self->partialExchangeHash);
    sha256_update(&self->partialExchangeHash, &hello, sizeof(hello) - 2);

    // Not implemented: The server may send other lines of data before the version string. rfc4253, 4.2.
    for (int32_t pos = 0;;) {
        if (_client_receive(self) <= 0) return -2;

        int32_t end = self->receivedSize;
        if (end > 255) end = 255; // rfc4253, 4.2.

        for (; pos + 1 < end; ++pos) {
            if (self->buffer[pos] == '\r') {
                if (self->buffer[pos + 1] != '\n') return -3;

                if (pos < (int64_t)(sizeof("SSH-2.0-") - 1) || hc_MEMCMP(&self->buffer[0], "SSH-2.0-", sizeof("SSH-2.0-") - 1) != 0) return -4;
                sha256_update(&self->partialExchangeHash, &(uint32_t) { hc_BSWAP32((uint32_t)pos) }, 4);
                sha256_update(&self->partialExchangeHash, &self->buffer[0], pos);
                self->bufferPos = pos + 2;
                self->receivedSize -= self->bufferPos;
                return 0;
            }
        }
    }
}

static int32_t _client_sendKeyExchangeInit(struct client *self) {
    #define _client_KEX_LIST_SIZE (sizeof(_client_KEX_LIST) - 1)
    #define _client_HOST_KEY_LIST_SIZE (sizeof(_client_HOST_KEY_LIST) - 1)
    #define _client_CIPHER_LIST_SIZE (sizeof(_client_CIPHER_LIST) - 1)
    #define _client_MAC_LIST ""
    #define _client_MAC_LIST_SIZE (sizeof(_client_MAC_LIST) - 1)
    #define _client_COMPRESSION_LIST "none"
    #define _client_COMPRESSION_LIST_SIZE (sizeof(_client_COMPRESSION_LIST) - 1)
    #define _client_LANGUAGE_LIST ""
    #define _client_LANGUAGE_LIST_SIZE (sizeof(_client_LANGUAGE_LIST) - 1)
    struct payload {
        uint8_t opcode;
        uint8_t cookie[16];
        uint32_t kexListSize;
        char kexList[_client_KEX_LIST_SIZE];
        uint32_t hostKeyListSize;
        char hostKeyList[_client_HOST_KEY_LIST_SIZE];
        uint32_t clientCipherListSize;
        char clientCipherList[_client_CIPHER_LIST_SIZE];
        uint32_t serverCipherListSize;
        char serverCipherList[_client_CIPHER_LIST_SIZE];
        uint32_t clientMacListSize;
        char clientMacList[_client_MAC_LIST_SIZE];
        uint32_t serverMacListSize;
        char serverMacList[_client_MAC_LIST_SIZE];
        uint32_t clientCompressionListSize;
        char clientCompressionList[_client_COMPRESSION_LIST_SIZE];
        uint32_t serverCompressionListSize;
        char serverCompressionList[_client_COMPRESSION_LIST_SIZE];
        uint32_t clientLanguageListSize;
        char clientLanguageList[_client_LANGUAGE_LIST_SIZE];
        uint32_t serverLanguageListSize;
        char serverLanguageList[_client_LANGUAGE_LIST_SIZE];
        uint8_t firstKexPacketFollows;
        uint32_t reserved;
    } hc_PACKED(1);

    struct packet {
        uint32_t size;
        uint8_t paddingSize;
        struct payload payload;
        uint8_t padding[_client_PADDING_SIZE(sizeof(struct payload))];
    } hc_PACKED(1);

    struct packet packet = {
        .size = hc_BSWAP32(sizeof(packet) - sizeof(packet.size)),
        .paddingSize = sizeof(packet.padding),
        .payload = {
            .opcode = proto_MSG_KEXINIT,
            .kexListSize = hc_BSWAP32(_client_KEX_LIST_SIZE),
            .kexList = _client_KEX_LIST,
            .hostKeyListSize = hc_BSWAP32(_client_HOST_KEY_LIST_SIZE),
            .hostKeyList = _client_HOST_KEY_LIST,
            .clientCipherListSize = hc_BSWAP32(_client_CIPHER_LIST_SIZE),
            .clientCipherList = _client_CIPHER_LIST,
            .serverCipherListSize = hc_BSWAP32(_client_CIPHER_LIST_SIZE),
            .serverCipherList = _client_CIPHER_LIST,
            .clientMacListSize = hc_BSWAP32(_client_MAC_LIST_SIZE),
            .clientMacList = _client_MAC_LIST,
            .serverMacListSize = hc_BSWAP32(_client_MAC_LIST_SIZE),
            .serverMacList = _client_MAC_LIST,
            .clientCompressionListSize = hc_BSWAP32(_client_COMPRESSION_LIST_SIZE),
            .clientCompressionList = _client_COMPRESSION_LIST,
            .serverCompressionListSize = hc_BSWAP32(_client_COMPRESSION_LIST_SIZE),
            .serverCompressionList = _client_COMPRESSION_LIST,
            .clientLanguageListSize = hc_BSWAP32(_client_LANGUAGE_LIST_SIZE),
            .clientLanguageList = _client_LANGUAGE_LIST,
            .serverLanguageListSize = hc_BSWAP32(_client_LANGUAGE_LIST_SIZE),
            .serverLanguageList = _client_LANGUAGE_LIST,
            .firstKexPacketFollows = 0,
            .reserved = 0
        }
    };

    if (sys_getrandom(&packet.payload.cookie, sizeof(packet.payload.cookie), 0) < 0) return -1;

    int64_t numSent = sys_sendto(self->socketFd, &packet, sizeof(packet), MSG_NOSIGNAL, NULL, 0);
    if (numSent != sizeof(packet)) return -2;

    self->tempHash = self->partialExchangeHash;
    sha256_update(&self->tempHash, &(uint32_t) { hc_BSWAP32(sizeof(packet.payload)) }, 4);
    sha256_update(&self->tempHash, &packet.payload, sizeof(packet.payload));
    return 0;
}

static bool _client_nameListContains(void *nameList, uint32_t nameListSize, void *string, uint32_t stringSize) {
    for (uint32_t i = 0;;) {
        if ((nameListSize - i) < stringSize) return false;
        if (hc_MEMCMP(nameList + i, string, stringSize) == 0) return true;
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
    struct ecdhInit {
        uint8_t opcode;
        uint32_t publicKeySize;
        uint8_t publicKey[32];
    } hc_PACKED(1);
    struct ecdhInitPacket {
        uint32_t size;
        uint8_t paddingSize;
        struct ecdhInit payload;
        uint8_t padding[_client_PADDING_SIZE(sizeof(struct ecdhInit))];
    } hc_PACKED(1);

    struct ecdhInitPacket ecdhInitPacket = {
        .size = hc_BSWAP32(sizeof(ecdhInitPacket) - sizeof(ecdhInitPacket.size)),
        .paddingSize = sizeof(ecdhInitPacket.padding),
        .payload = {
            .opcode = proto_MSG_KEX_ECDH_INIT,
            .publicKeySize = hc_BSWAP32(sizeof(ecdhInitPacket.payload.publicKey))
        }
    };
    uint8_t secret[32];
    if (sys_getrandom(&secret[0], sizeof(secret), 0) < 0) return -12;
    x25519(&ecdhInitPacket.payload.publicKey[0], &secret[0], &x25519_ecdhBasepoint[0]);

    int64_t numSent = sys_sendto(self->socketFd, &ecdhInitPacket, sizeof(ecdhInitPacket), MSG_NOSIGNAL, NULL, 0);
    if (numSent != sizeof(ecdhInitPacket)) return -13;

    // Wait for server's KEX_ECDH_REPLY.
    struct ecdhReply {
        uint8_t opcode;
        uint32_t hostKeySize;
        struct {
            uint32_t keyTypeSize;
            uint8_t keyType[sizeof(_client_HOST_KEY_ED25519) - 1];
            uint32_t publicKeySize;
            uint8_t publicKey[32];
        } hc_PACKED(1) hostKey;
        uint32_t publicKeySize;
        uint8_t publicKey[32];
        uint32_t fullSignatureSize;
        struct {
            uint32_t signatureTypeSize;
            uint8_t signatureType[sizeof(_client_HOST_KEY_ED25519) - 1];
            uint32_t signatureSize;
            uint8_t signature[ed25519_SIGNATURE_SIZE];
        } hc_PACKED(1) fullSignature;
    } hc_PACKED(1);
    struct ecdhReply *ecdhReply;
    int32_t ecdhReplySize = _client_waitForPacket(self, (uint8_t **)&ecdhReply);
    if (ecdhReplySize != sizeof(struct ecdhReply)) return -14;
    if (
        ecdhReply->opcode != proto_MSG_KEX_ECDH_REPLY ||
        ecdhReply->hostKeySize != hc_BSWAP32(sizeof(ecdhReply->hostKey)) ||
        ecdhReply->hostKey.keyTypeSize != hc_BSWAP32(sizeof(ecdhReply->hostKey.keyType)) ||
        hc_MEMCMP(ecdhReply->hostKey.keyType, _client_HOST_KEY_ED25519, sizeof(ecdhReply->hostKey.keyType)) != 0 ||
        ecdhReply->hostKey.publicKeySize != hc_BSWAP32(32) ||
        ecdhReply->publicKeySize != hc_BSWAP32(32) ||
        ecdhReply->fullSignatureSize != hc_BSWAP32(sizeof(ecdhReply->fullSignature)) ||
        ecdhReply->fullSignature.signatureTypeSize != hc_BSWAP32(sizeof(ecdhReply->fullSignature.signatureType)) ||
        hc_MEMCMP(ecdhReply->fullSignature.signatureType, _client_HOST_KEY_ED25519, sizeof(ecdhReply->fullSignature.signatureType)) != 0 ||
        ecdhReply->fullSignature.signatureSize != hc_BSWAP32(sizeof(ecdhReply->fullSignature.signature))
    ) return -15;

    // Calculate shared secret.
    uint8_t sharedSecret[32];
    x25519(&sharedSecret[0], &secret[0], &ecdhReply->publicKey[0]);

    // Finish exchange hash.
    uint8_t exchangeHash[sha256_HASH_SIZE];
    sha256_update(&self->tempHash, &ecdhReply->hostKeySize, sizeof(ecdhReply->hostKeySize) + sizeof(ecdhReply->hostKey));
    sha256_update(&self->tempHash, &ecdhInitPacket.payload.publicKeySize, sizeof(ecdhInitPacket.payload.publicKeySize) + sizeof(ecdhInitPacket.payload.publicKey));
    sha256_update(&self->tempHash, &ecdhReply->publicKeySize, sizeof(ecdhReply->publicKeySize) + sizeof(ecdhReply->publicKey));
    _client_sha256UpdateSharedSecret(&self->tempHash, &sharedSecret[0]);
    sha256_finish(&self->tempHash, &exchangeHash[0]);

    // Verify signature.
    int32_t status = ed25519_verify(&exchangeHash[0], sha256_HASH_SIZE, &ecdhReply->hostKey.publicKey[0], &ecdhReply->fullSignature.signature[0]);
    if (status != 0) return -16;

    // Set session id.
    if (self->encryption == _client_encryption_NONE) hc_MEMCPY(&self->sessionId[0], &exchangeHash[0], sizeof(self->sessionId));

    // Send NEWKEYS message.
    struct newKeys {
        uint8_t opcode;
    };
    struct newKeysPacket {
        uint32_t size;
        uint8_t paddingSize;
        struct newKeys payload;
        uint8_t padding[_client_PADDING_SIZE(sizeof(struct newKeys))];
    } hc_PACKED(1);

    struct newKeysPacket newKeysPacket = {
        .size = hc_BSWAP32(sizeof(newKeysPacket) - sizeof(newKeysPacket.size)),
        .paddingSize = sizeof(newKeysPacket.padding),
        .payload = {
            .opcode = proto_MSG_NEWKEYS
        }
    };
    numSent = sys_sendto(self->socketFd, &newKeysPacket, sizeof(newKeysPacket), MSG_NOSIGNAL, NULL, 0);
    if (numSent != sizeof(newKeysPacket)) return -17;

    // Wait for server's NEWKEYS message.
    uint8_t *serverNewKeys;
    int32_t serverNewKeysSize = _client_waitForPacket(self, &serverNewKeys);
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

        // The key length ciphers always use block counter of 0, so set it now.
        self->ciphers[2 * i + 1].orig.blockCounter = 0;
    }
    return 0;
}

static int32_t client_init(struct client *self, int32_t sockaddrFamily) {
    for (int32_t i = 0; i < (int32_t)hc_ARRAY_LEN(self->ciphers); ++i) {
        chacha20_init(&self->ciphers[i]);
    }

    // Create circular buffer.
    self->bufferMemFd = sys_memfd_create("", MFD_CLOEXEC);
    if (self->bufferMemFd < 0) return -1;

    self->bufferSize = math_ALIGN_FORWARD(35000, (int32_t)client_PAGE_SIZE); // rfc4253, 6.1.
    int32_t status = sys_ftruncate(self->bufferMemFd, self->bufferSize);
    if (status < 0) {
        status = -2;
        goto cleanup_bufferMemFd;
    }

    self->buffer = sys_mmap(NULL, 2 * self->bufferSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
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

static int32_t client_connect(struct client *self, void *sockaddr, int32_t sockaddrSize) {
    self->bufferPos = 0;
    self->receivedSize = 0;
    self->receiveSequenceNumber = 0;
    self->sendSequenceNumber = 0;
    self->encryption = _client_encryption_NONE;

    // Connect to server.
    int32_t status = sys_connect(self->socketFd, sockaddr, sockaddrSize);
    if (status < 0) return -1;

    status = _client_doHello(self);
    if (status < 0) return -2;

    status = _client_sendKeyExchangeInit(self);
    if (status < 0) return -3;

    // Receive next packet (should be a KEXINIT).
    uint8_t *serverPacket;
    int32_t serverPacketSize = _client_waitForPacket(self, &serverPacket);
    if (serverPacketSize <= 0) return -4;

    // Perform key exchange.
    status = _client_doKeyExchange(self, serverPacket, serverPacketSize);
    if (status < 0) {
        debug_printNum("Key exchange failed (", status, ")\n");
        return -5;
    }

    // TODO: To be continued...
    return 0;
}

static void client_deinit(struct client *self) {
    debug_CHECK(sys_close(self->socketFd), RES == 0);
    debug_CHECK(sys_munmap(self->buffer, 2 * self->bufferSize), RES == 0);
    debug_CHECK(sys_close(self->bufferMemFd), RES == 0);
}

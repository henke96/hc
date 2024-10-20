#define dhcpServer_FIRST_OCTET 40
#define dhcpServer_NUM_ENTRIES (255 - dhcpServer_FIRST_OCTET)
static_assert(dhcpServer_FIRST_OCTET > 1 && dhcpServer_NUM_ENTRIES > 0, "");

struct dhcpServer_entry {
    int64_t timeSec; // 0 if unused.
    uint8_t macAddr[6];
    int16_t __pad;
};

struct dhcpServer {
    struct ifreq ifreq; // ifr_name and ifr_addr.
    uint32_t serverIp;
    int32_t fd;
    int32_t ifIndex;
    int32_t __pad;
    struct dhcpServer_entry entries[dhcpServer_NUM_ENTRIES];
};

// Updates or creates entry for mac address, and returns index to it.
// If `allowCreate` is false and the entry doesn't exist, -1 is returned.
static int32_t dhcpServer_getEntry(struct dhcpServer *self, uint8_t *macAddr, bool allowCreate) {
    struct timespec currentTime = {0};
    debug_CHECK(sys_clock_gettime(CLOCK_MONOTONIC, &currentTime), RES == 0);

    int32_t i = 0;
    int32_t oldestIndex = i;
    int64_t oldestTimeSec = self->entries[i].timeSec;
    for (;;) {
        if (mem_compare(&macAddr[0], &self->entries[i].macAddr[0], 6) == 0) {
            self->entries[i].timeSec = currentTime.tv_sec;
            return i;
        }
        if (++i == dhcpServer_NUM_ENTRIES) break;
        if (self->entries[i].timeSec < oldestTimeSec) {
            oldestIndex = i;
            oldestTimeSec = self->entries[i].timeSec;
        }
    }

    if (!allowCreate) return -1;

    hc_MEMCPY(&self->entries[oldestIndex].macAddr[0], &macAddr[0], 6);
    self->entries[oldestIndex].timeSec = currentTime.tv_sec;
    return oldestIndex;
}

static void dhcpServer_init(struct dhcpServer *self, int32_t ifIndex, uint32_t serverIp) {
    self->ifIndex = ifIndex;
    self->serverIp = serverIp;
    for (int32_t i = 0; i < dhcpServer_NUM_ENTRIES; ++i) self->entries[i].timeSec = 0;

    CHECK(self->fd = sys_socket(AF_INET, SOCK_DGRAM, 0), RES > 0);

    // Populate interface name and MAC address.
    self->ifreq.ifr_ifindex = ifIndex;
    CHECK(sys_ioctl(self->fd, SIOCGIFNAME, &self->ifreq), RES == 0);
    CHECK(sys_ioctl(self->fd, SIOCGIFHWADDR, &self->ifreq), RES == 0);

    // Bind the socket to the interface.
    CHECK(sys_setsockopt(self->fd, SOL_SOCKET, SO_BINDTODEVICE, &self->ifreq.ifr_name, IFNAMSIZ), RES == 0);

    // Allow sending/receiving multicast.
    int32_t broadcast = 1;
    CHECK(sys_setsockopt(self->fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)), RES == 0);

    // Bind to any IP, port 67.
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(67),
        .sin_addr = { 0, 0, 0, 0 }
    };
    CHECK(sys_bind(self->fd, &addr, sizeof(addr)), RES == 0);
}

static void dhcpServer_onFd(struct dhcpServer *self) {
    int64_t read = sys_read(self->fd, &buffer[0], sizeof(buffer));
    debug_ASSERT(read > 0);
    if (read < (int64_t)sizeof(struct dhcp_header)) return;
    void *end = &buffer[read];
    struct dhcp_header *header = (void *)&buffer[0];

    // Find DHCP options.
    struct dhcp_option *messageType = NULL;
    for (struct dhcp_option *current = (void *)&header[1];;) {
        void *next = &((char *)current)[sizeof(*current) + current->length];
        if (next > end) break;
        switch (current->code) {
            case dhcp_MESSAGE_TYPE: {
                if (current->length == 1) messageType = current;
                break;
            }
            default: break;
        }
        current = next;
    }
    if (messageType == NULL) return;

    uint8_t type = *(uint8_t *)&messageType[1];
    bool isDiscover = type == dhcp_DISCOVER;
    if (!isDiscover && type != dhcp_REQUEST) return;

    int32_t entryIndex = dhcpServer_getEntry(self, &header->clientHwAddr[0], isDiscover);
    if (entryIndex < 0) return;

    struct reply {
        struct dhcp_header hdr;
        struct dhcp_option messageTypeOpt;
        uint8_t messageType;
        struct dhcp_option serverIdOpt;
        uint8_t serverId[4];
        struct dhcp_option subnetMaskOpt;
        uint8_t subnetMask[4];
        struct dhcp_option routerOpt;
        uint8_t router[4];
        struct dhcp_option leaseTimeOpt;
        uint8_t leaseTime[4];
        struct dhcp_option dnsOpt;
        uint8_t dns[4];
        uint8_t endOpt;
        uint8_t __pad[2];
    };

    struct reply replyMsg = {
        .hdr = {
            .opcode = dhcp_BOOTREPLY,
            .hwAddrType = dhcp_ETHERNET,
            .hwAddrLen = 6,
            .identifier = header->identifier,
            .yourIp = { 0, 0, 0, (uint8_t)(dhcpServer_FIRST_OCTET + entryIndex) },
            .magicCookie = dhcp_MAGIC_COOKIE
        },
        .messageTypeOpt = {
            .code = dhcp_MESSAGE_TYPE,
            .length = sizeof(replyMsg.messageType)
        },
        .messageType = isDiscover ? dhcp_OFFER : dhcp_ACK,
        .serverIdOpt = {
            .code = dhcp_SERVER_IDENTIFIER,
            .length = sizeof(replyMsg.serverId)
        },
        .subnetMaskOpt = {
            .code = dhcp_SUBNET_MASK,
            .length = sizeof(replyMsg.subnetMask)
        },
        .subnetMask = { 255, 255, 255, 0 },
        .routerOpt = {
            .code = dhcp_ROUTER,
            .length = sizeof(replyMsg.router)
        },
        .leaseTimeOpt = {
            .code = dhcp_LEASE_TIME,
            .length = sizeof(replyMsg.leaseTime)
        },
        .leaseTime = { 0, 0, 2, 88 }, // 2 * 256 + 88 = 600 seconds.
        .dnsOpt = {
            .code = dhcp_DNS,
            .length = sizeof(replyMsg.dns)
        },
        .dns = { 8, 8, 8, 8 },
        .endOpt = dhcp_END
    };
    hc_MEMCPY(&replyMsg.hdr.yourIp[0], &self->serverIp, 3);
    hc_MEMCPY(&replyMsg.hdr.serverIp[0], &self->serverIp, 4);
    hc_MEMCPY(&replyMsg.serverId[0], &self->serverIp, 4);
    hc_MEMCPY(&replyMsg.router[0], &self->serverIp, 4);

    hc_MEMCPY(&replyMsg.hdr.clientHwAddr[0], &header->clientHwAddr[0], 6);
    if (dhcpClient.dnsIp != 0) hc_MEMCPY(&replyMsg.dns, &dhcpClient.dnsIp, sizeof(replyMsg.dns));

    struct sockaddr_in destAddr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(68),
        .sin_addr = { 255, 255, 255, 255 }
    };
    // Unicast if the client says it already has the IP.
    if (mem_compare(&header->clientIp[0], &replyMsg.hdr.yourIp[0], 4) == 0) hc_MEMCPY(&destAddr.sin_addr[0], &replyMsg.hdr.yourIp[0], 4);

    debug_CHECK(sys_sendto(self->fd, &replyMsg, sizeof(replyMsg), MSG_NOSIGNAL, &destAddr, sizeof(destAddr)), RES == sizeof(replyMsg));
}

static void dhcpServer_deinit(struct dhcpServer *self) {
    debug_CHECK(sys_close(self->fd), RES == 0);
}

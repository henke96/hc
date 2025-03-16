struct dhcpClient {
    int32_t fd;
    int32_t packetFd; // For sending discovers.
    int32_t timerFd;
    int32_t dnsIp; // 0 if none.
    uint32_t currentIdentifier;
    uint32_t leasedIp; // Currently leased ip, 0 if none.
    uint32_t renewServerIp; // The server to renew the lease from.
    int32_t ifIndex;
    uint32_t routePriority;
    char ifName[IFNAMSIZ];
    uint8_t ifMacAddress[6];
    uint8_t leasedIpNetmask; // 0 if none.
    char __pad;
};

static void dhcpClient_init(struct dhcpClient *self, int32_t ifIndex, uint32_t routePriority) {
    self->dnsIp = 0;
    self->currentIdentifier = 0;
    self->leasedIp = 0;
    self->renewServerIp = 0;
    self->leasedIpNetmask = 0;
    self->ifIndex = ifIndex;
    self->routePriority = routePriority;

    self->fd = sys_socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(self->fd, RES > 0);

    // Populate interface name and MAC address.
    struct ifreq ifreq = { .ifr_ifindex = self->ifIndex };
    CHECK(sys_ioctl(self->fd, SIOCGIFNAME, &ifreq), RES == 0);
    CHECK(sys_ioctl(self->fd, SIOCGIFHWADDR, &ifreq), RES == 0);
    hc_MEMCPY(&self->ifName[0], &ifreq.ifr_name[0], sizeof(self->ifName));
    hc_MEMCPY(&self->ifMacAddress[0], &ifreq.ifr_addr[2], sizeof(self->ifMacAddress));

    // Bind the socket to the interface.
    CHECK(sys_setsockopt(self->fd, SOL_SOCKET, SO_BINDTODEVICE, &self->ifName[0], IFNAMSIZ), RES == 0);

    // Allow sending/receiving multicast.
    int32_t broadcast = 1;
    CHECK(sys_setsockopt(self->fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)), RES == 0);

    // Bind to any IP, port 68.
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(68),
        .sin_addr = { 0, 0, 0, 0 }
    };
    CHECK(sys_bind(self->fd, &addr, sizeof(addr)), RES == 0);

    // Create packet socket for sending discovers.
    self->packetFd = sys_socket(AF_PACKET, SOCK_DGRAM, 0);
    CHECK(self->packetFd, RES > 0);

    // Create timer fd, and set initial timeout.
    self->timerFd = sys_timerfd_create(CLOCK_MONOTONIC, 0);
    CHECK(self->timerFd, RES > 0);

    struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
    CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
}

static void dhcpClient_onTimerFd(struct dhcpClient *self) {
    uint64_t expirations = 0;
    CHECK(sys_read(self->timerFd, &expirations, sizeof(expirations)), RES == sizeof(expirations));
    debug_ASSERT(expirations == 1);

    struct {
        struct dhcp_header hdr;
        struct dhcp_option messageTypeOpt;
        uint8_t messageType;
        struct dhcp_option paramRequestListOpt;
        uint8_t paramRequestList[3];
        uint8_t endOpt;
        uint8_t __pad[3];
    } requestMsg = {
        .hdr = {
            .opcode = dhcp_BOOTREQUEST,
            .hwAddrType = dhcp_ETHERNET,
            .hwAddrLen = 6,
            .identifier = hc_BSWAP32(++self->currentIdentifier),
            .magicCookie = dhcp_MAGIC_COOKIE
        },
        .messageTypeOpt = {
            .code = dhcp_MESSAGE_TYPE,
            .length = sizeof(requestMsg.messageType)
        },
        .paramRequestListOpt = {
            .code = dhcp_PARAM_REQUEST_LIST,
            .length = sizeof(requestMsg.paramRequestList)
        },
        .paramRequestList = { dhcp_SUBNET_MASK, dhcp_ROUTER, dhcp_DNS },
        .endOpt = dhcp_END
    };
    hc_MEMCPY(&requestMsg.hdr.clientHwAddr, &self->ifMacAddress[0], sizeof(self->ifMacAddress));

    if (self->leasedIp != 0 && self->renewServerIp != 0) {
        // Attempt to renew the leased IP once.
        requestMsg.messageType = dhcp_REQUEST;
        hc_MEMCPY(&requestMsg.hdr.clientIp[0], &self->leasedIp, 4);

        struct sockaddr_in destAddr = {
            .sin_family = AF_INET,
            .sin_port = hc_BSWAP16(67)
        };
        hc_MEMCPY(&destAddr.sin_addr[0], &self->renewServerIp, 4);
        debug_CHECK(
            sys_sendto(self->fd, &requestMsg, sizeof(requestMsg), MSG_NOSIGNAL, &destAddr, sizeof(destAddr)),
            RES == sizeof(requestMsg)
        );
        self->renewServerIp = 0;
    } else {
        // Either we had no leased IP, or we failed to renew it.
        if (self->leasedIp != 0) {
            char print[sizeof(self->ifName) + hc_STR_LEN(" lost IP lease\n")];
            char *pos = hc_ARRAY_END(print);
            hc_PREPEND_STR(pos, " lost IP lease\n");
            hc_PREPEND(pos, &self->ifName[0], (size_t)util_cstrLen(&self->ifName[0]));
            debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);

            // Remove the IP.
            struct addrRequest {
                struct nlmsghdr hdr;
                struct ifaddrmsg addrMsg;
                struct nlattr addrAttr;
                uint8_t address[4];
            };
            struct addrRequest request = {
                .hdr = {
                    .nlmsg_len = sizeof(request),
                    .nlmsg_type = RTM_DELADDR,
                    .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
                },
                .addrMsg = {
                    .ifa_family = AF_INET,
                    .ifa_index = self->ifIndex
                },
                .addrAttr = {
                    .nla_len = sizeof(request.addrAttr) + sizeof(request.address),
                    .nla_type = IFA_LOCAL
                }
            };
            hc_MEMCPY(&request.address, &self->leasedIp, 4);
            struct iovec_const iov[] = { { &request, sizeof(request) } };
            netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            self->leasedIp = 0;
        }
        requestMsg.messageType = dhcp_DISCOVER;
        requestMsg.hdr.flags = hc_BSWAP16(0x8000); // Request broadcast responses.

        struct {
            uint8_t versionIhl;
            uint8_t dscpEcn;
            uint16_t totalLength;
            uint16_t identification;
            uint16_t flagsFragmentOffset;
            uint8_t timeToLive;
            uint8_t protocol;
            uint16_t checksum;
            uint32_t sourceAddress;
            uint32_t destinationAddress;
            struct {
                uint16_t sourcePort;
                uint16_t destinationPort;
                uint16_t length;
                uint16_t checksum;
            } udp;
        } ipUdpHeader = {
            .versionIhl = 0x45,
            .dscpEcn = 0,
            .totalLength = hc_BSWAP16(sizeof(ipUdpHeader) + sizeof(requestMsg)),
            .identification = 0,
            .flagsFragmentOffset = hc_BSWAP16(2 << 13), // DF
            .timeToLive = 64,
            .protocol = 17, // UDP
            .checksum = hc_BSWAP16(0x39d6), // Pre-calculated.
            .sourceAddress = 0,
            .destinationAddress = 0xFFFFFFFF,
            .udp = {
                .sourcePort = hc_BSWAP16(68),
                .destinationPort = hc_BSWAP16(67),
                .length = hc_BSWAP16(sizeof(ipUdpHeader.udp) + sizeof(requestMsg)),
                .checksum = 0 // UDP checksum is optional for IPv4.
            }
        };

        struct sockaddr_ll addr = {
            .sll_family = AF_PACKET,
            .sll_protocol = hc_BSWAP16(ETH_P_IP),
            .sll_ifindex = self->ifIndex,
            .sll_halen = 6,
            .sll_addr = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
        };
        struct iovec_const iov[] = {
            { .iov_base = &ipUdpHeader, .iov_len = sizeof(ipUdpHeader) },
            { .iov_base = &requestMsg, .iov_len = sizeof(requestMsg) }
        };
        struct msghdr_const sendMsghdr = {
            .msg_name = &addr,
            .msg_namelen = sizeof(addr),
            .msg_iov = &iov[0],
            .msg_iovlen = hc_ARRAY_LEN(iov)
        };
        debug_CHECK(
            sys_sendmsg(self->packetFd, &sendMsghdr, MSG_NOSIGNAL),
            RES == (int64_t)sizeof(ipUdpHeader) + sizeof(requestMsg)
        );
    }

    // Set timeout to 10 seconds.
    struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
    CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
}

static void dhcpClient_onFd(struct dhcpClient *self) {
    int64_t numRead = sys_read(self->fd, &buffer[0], sizeof(buffer));
    debug_ASSERT(numRead > 0);
    if (numRead < (int64_t)sizeof(struct dhcp_header)) return;
    void *end = &buffer[numRead];
    struct dhcp_header *header = (void *)&buffer[0];

    // Make sure the packet is for us.
    if (mem_compare(&header->clientHwAddr, &self->ifMacAddress[0], sizeof(self->ifMacAddress)) != 0) return;

    // Find DHCP options.
    struct dhcp_option *messageType = NULL;
    struct dhcp_option *subnetMask = NULL;
    struct dhcp_option *router = NULL;
    struct dhcp_option *dns = NULL;
    struct dhcp_option *leaseTime = NULL;
    for (struct dhcp_option *current = (void *)&header[1];;) {
        void *next = (void *)current + sizeof(*current) + current->length;
        if (next > end) break;
        switch (current->code) {
            case dhcp_MESSAGE_TYPE: {
                if (current->length == 1) messageType = current;
                break;
            }
            case dhcp_SUBNET_MASK: {
                if (current->length == 4) subnetMask = current;
                break;
            }
            case dhcp_ROUTER: {
                if (current->length >= 4) router = current;
                break;
            }
            case dhcp_DNS: {
                if (current->length >= 4) dns = current;
                break;
            }
            case dhcp_LEASE_TIME: {
                if (current->length == 4) leaseTime = current;
                break;
            }
            default: break;
        }
        current = next;
    }
    if (messageType == NULL) return;

    switch (*(uint8_t *)&messageType[1]) {
        case dhcp_OFFER: {
            if (leaseTime == NULL) return;

            // Respond to the offer with a request.
            struct {
                struct dhcp_header hdr;
                struct dhcp_option messageTypeOpt;
                uint8_t messageType;
                struct dhcp_option paramRequestListOpt;
                uint8_t paramRequestList[3];
                struct dhcp_option leaseTimeOpt;
                uint8_t leaseTime[4];
                struct dhcp_option serverIdOpt;
                uint8_t serverId[4];
                struct dhcp_option requestedIpOpt;
                uint8_t requestedIp[4];
                uint8_t endOpt;
                uint8_t __pad;
            } requestMsg = {
                .hdr = {
                    .opcode = dhcp_BOOTREQUEST,
                    .hwAddrType = dhcp_ETHERNET,
                    .hwAddrLen = 6,
                    .identifier = hc_BSWAP32(self->currentIdentifier),
                    .flags = hc_BSWAP16(0x8000), // Request broadcast responses.
                    .magicCookie = dhcp_MAGIC_COOKIE
                },
                .messageTypeOpt = {
                    .code = dhcp_MESSAGE_TYPE,
                    .length = sizeof(requestMsg.messageType)
                },
                .messageType = dhcp_REQUEST,
                .paramRequestListOpt = {
                    .code = dhcp_PARAM_REQUEST_LIST,
                    .length = sizeof(requestMsg.paramRequestList)
                },
                .paramRequestList = { dhcp_SUBNET_MASK, dhcp_ROUTER, dhcp_DNS },
                .leaseTimeOpt = {
                    .code = dhcp_LEASE_TIME,
                    .length = sizeof(requestMsg.leaseTime)
                },
                .serverIdOpt = {
                    .code = dhcp_SERVER_IDENTIFIER,
                    .length = sizeof(requestMsg.serverId)
                },
                .requestedIpOpt = {
                    .code = dhcp_REQUESTED_IP_ADDRESS,
                    .length = sizeof(requestMsg.requestedIp)
                },
                .endOpt = dhcp_END
            };
            hc_MEMCPY(&requestMsg.leaseTime, &leaseTime[1], sizeof(requestMsg.leaseTime));
            hc_MEMCPY(&requestMsg.serverId, &header->serverIp, sizeof(requestMsg.serverId));
            hc_MEMCPY(&requestMsg.hdr.clientHwAddr, &self->ifMacAddress[0], sizeof(self->ifMacAddress));
            hc_MEMCPY(&requestMsg.requestedIp, &header->yourIp, sizeof(requestMsg.requestedIp));

            struct sockaddr_in destAddr = {
                .sin_family = AF_INET,
                .sin_port = hc_BSWAP16(67),
                .sin_addr = { 255, 255, 255, 255 }
            };
            debug_CHECK(sys_sendto(self->fd, &requestMsg, sizeof(requestMsg), MSG_NOSIGNAL, &destAddr, sizeof(destAddr)), RES == sizeof(requestMsg));

            // Set timeout to 10 seconds.
            struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
            CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
            break;
        }
        case dhcp_NAK: {
            // Something didn't succeed, retry in a few seconds.
            struct itimerspec timeout = { .it_value = { .tv_sec = 5 } };
            CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);
            break;
        }
        case dhcp_ACK: {
            if (subnetMask == NULL || router == NULL || leaseTime == NULL) return;
            if (dns != NULL) {
                hc_MEMCPY(&self->dnsIp, &dns[1], 4);
            } else self->dnsIp = 0;

            uint32_t leaseTimeValue;
            hc_MEMCPY(&leaseTimeValue, &leaseTime[1], 4);
            leaseTimeValue = hc_BSWAP32(leaseTimeValue);

            // Set timeout to half of the lease time, rounded up.
            struct itimerspec timeout = { .it_value = { .tv_sec = (leaseTimeValue + 1) / 2 } };
            CHECK(sys_timerfd_settime(self->timerFd, 0, &timeout, NULL), RES == 0);

            uint32_t netmask;
            hc_MEMCPY(&netmask, &subnetMask[1], 4);
            self->leasedIpNetmask = (uint8_t)hc_POPCOUNT32(netmask);

            // Record new IP lease and renew-server.
            hc_MEMCPY(&self->renewServerIp, &header->serverIp, 4);
            if (mem_compare(&self->leasedIp, &header->yourIp[0], 4) != 0) {
                hc_MEMCPY(&self->leasedIp, &header->yourIp[0], 4);

                char print[
                    sizeof(self->ifName) +
                    hc_STR_LEN(" leased IP: ") +
                    util_UINT8_MAX_CHARS + hc_STR_LEN(".") +
                    util_UINT8_MAX_CHARS + hc_STR_LEN(".") +
                    util_UINT8_MAX_CHARS + hc_STR_LEN(".") +
                    util_UINT8_MAX_CHARS + hc_STR_LEN("/") +
                    util_UINT8_MAX_CHARS + hc_STR_LEN("\n")
                ];
                char *pos = hc_ARRAY_END(print);
                hc_PREPEND_STR(pos, "\n");
                pos = util_uintToStr(pos, self->leasedIpNetmask);
                hc_PREPEND_STR(pos, "/");
                pos = util_uintToStr(pos, ((uint8_t *)&self->leasedIp)[3]);
                hc_PREPEND_STR(pos, ".");
                pos = util_uintToStr(pos, ((uint8_t *)&self->leasedIp)[2]);
                hc_PREPEND_STR(pos, ".");
                pos = util_uintToStr(pos, ((uint8_t *)&self->leasedIp)[1]);
                hc_PREPEND_STR(pos, ".");
                pos = util_uintToStr(pos, ((uint8_t *)&self->leasedIp)[0]);
                hc_PREPEND_STR(pos, " leased IP: ");
                hc_PREPEND(pos, &self->ifName[0], (size_t)util_cstrLen(&self->ifName[0]));
                debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
            }

            // Add the address.
            {
                struct addrRequest {
                    struct nlmsghdr hdr;
                    struct ifaddrmsg addrMsg;
                    struct nlattr addrAttr;
                    uint8_t address[4];
                };
                struct addrRequest request = {
                    .hdr = {
                        .nlmsg_len = sizeof(request),
                        .nlmsg_type = RTM_NEWADDR,
                        .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_REPLACE,
                    },
                    .addrMsg = {
                        .ifa_family = AF_INET,
                        .ifa_prefixlen = self->leasedIpNetmask,
                        .ifa_index = self->ifIndex
                    },
                    .addrAttr = {
                        .nla_len = sizeof(request.addrAttr) + sizeof(request.address),
                        .nla_type = IFA_LOCAL
                    }
                };
                hc_MEMCPY(&request.address, &header->yourIp[0], 4);
                struct iovec_const iov[] = { { &request, sizeof(request) } };
                netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            }

            // Set default route.
            {
                struct routeRequest {
                    struct nlmsghdr hdr;
                    struct rtmsg routeMsg;
                    struct nlattr gatewayAttr;
                    uint8_t gateway[4];
                    struct nlattr priorityAttr;
                    uint32_t priority;
                };
                struct routeRequest request = {
                    .hdr = {
                        .nlmsg_len = sizeof(request),
                        .nlmsg_type = RTM_NEWROUTE,
                        .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_REPLACE,
                    },
                    .routeMsg = {
                        .rtm_family = AF_INET,
                        .rtm_dst_len = 0,
                        .rtm_table = RT_TABLE_MAIN,
                        .rtm_scope = RT_SCOPE_UNIVERSE,
                        .rtm_type = RTN_UNICAST
                    },
                    .gatewayAttr = {
                        .nla_len = sizeof(request.gatewayAttr) + sizeof(request.gateway),
                        .nla_type = RTA_GATEWAY
                    },
                    .priorityAttr = {
                        .nla_len = sizeof(request.priorityAttr) + sizeof(request.priority),
                        .nla_type = RTA_PRIORITY
                    },
                    .priority = self->routePriority
                };
                hc_MEMCPY(&request.gateway, &router[1], sizeof(request.gateway));
                struct iovec_const iov[] = { { &request, sizeof(request) } };
                netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            }
            break;
        }
        default: break;
    }
}

static void dhcpClient_deinit(struct dhcpClient *self) {
    debug_CHECK(sys_close(self->timerFd), RES == 0);
    debug_CHECK(sys_close(self->fd), RES == 0);
}

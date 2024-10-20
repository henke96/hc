#define dhcp_IFINDEX config_WAN_IF_INDEX

struct dhcpClient {
    struct ifreq ifreq; // ifr_name and ifr_addr.
    int32_t fd;
    int32_t timerFd;
    int32_t dnsIp; // 0 if none.
    uint32_t currentIdentifier;
    uint32_t leasedIp; // Currently leased ip, 0 if none.
    int32_t leasedIpNetmask; // 0 if none.
    uint32_t renewServerIp; // The server to renew the lease from.
    char __pad[4];
};

static struct dhcpClient dhcpClient = { 0 };

static void dhcpClient_init(void) {
    dhcpClient.fd = sys_socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(dhcpClient.fd, RES > 0);

    // Populate interface name and MAC address.
    dhcpClient.ifreq.ifr_ifindex = dhcp_IFINDEX;
    CHECK(sys_ioctl(dhcpClient.fd, SIOCGIFNAME, &dhcpClient.ifreq), RES == 0);
    CHECK(sys_ioctl(dhcpClient.fd, SIOCGIFHWADDR, &dhcpClient.ifreq), RES == 0);

    // Bind the socket to the interface.
    CHECK(sys_setsockopt(dhcpClient.fd, SOL_SOCKET, SO_BINDTODEVICE, &dhcpClient.ifreq.ifr_name, IFNAMSIZ), RES == 0);

    // Allow sending/receiving multicast.
    int32_t broadcast = 1;
    CHECK(sys_setsockopt(dhcpClient.fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)), RES == 0);

    // Bind to any IP, port 68.
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(68),
        .sin_addr = { 0, 0, 0, 0 }
    };
    CHECK(sys_bind(dhcpClient.fd, &addr, sizeof(addr)), RES == 0);

    // Create timer fd, and set initial timeout.
    dhcpClient.timerFd = sys_timerfd_create(CLOCK_MONOTONIC, 0);
    CHECK(dhcpClient.timerFd, RES > 0);

    struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
    CHECK(sys_timerfd_settime(dhcpClient.timerFd, 0, &timeout, NULL), RES == 0);
}

static void dhcpClient_onTimerFd(void) {
    uint64_t expirations = 0;
    CHECK(sys_read(dhcpClient.timerFd, &expirations, sizeof(expirations)), RES == sizeof(expirations));
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
            .identifier = hc_BSWAP32(++dhcpClient.currentIdentifier),
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
    hc_MEMCPY(&requestMsg.hdr.clientHwAddr, &dhcpClient.ifreq.ifr_addr[2], 6);

    struct sockaddr_in destAddr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(67),
        .sin_addr = { 255, 255, 255, 255 }
    };

    if (dhcpClient.leasedIp != 0 && dhcpClient.renewServerIp != 0) {
        // Attempt to renew the leased IP once.
        requestMsg.messageType = dhcp_REQUEST;
        hc_MEMCPY(&requestMsg.hdr.clientIp[0], &dhcpClient.leasedIp, 4);
        hc_MEMCPY(&destAddr.sin_addr[0], &dhcpClient.renewServerIp, 4);
        dhcpClient.renewServerIp = 0;
    } else {
        // Either we had no leased IP, or we failed to renew it.
        if (dhcpClient.leasedIp != 0) {
            sys_write(2, hc_STR_COMMA_LEN("Lost IP lease\n"));

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
                    .ifa_index = dhcp_IFINDEX
                },
                .addrAttr = {
                    .nla_len = sizeof(request.addrAttr) + sizeof(request.address),
                    .nla_type = IFA_LOCAL
                }
            };
            hc_MEMCPY(&request.address, &dhcpClient.leasedIp, 4);
            struct iovec_const iov[] = { { &request, sizeof(request) } };
            netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            dhcpClient.leasedIp = 0;
        }
        requestMsg.messageType = dhcp_DISCOVER;
        requestMsg.hdr.flags = hc_BSWAP16(0x8000); // Request broadcast responses.
    }
    debug_CHECK(sys_sendto(dhcpClient.fd, &requestMsg, sizeof(requestMsg), MSG_NOSIGNAL, &destAddr, sizeof(destAddr)), RES == sizeof(requestMsg));

    // Set timeout to 10 seconds.
    struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
    CHECK(sys_timerfd_settime(dhcpClient.timerFd, 0, &timeout, NULL), RES == 0);
}

static void dhcpClient_onFd(void) {
    int64_t read = sys_read(dhcpClient.fd, &buffer[0], sizeof(buffer));
    debug_ASSERT(read > 0);
    if (read < (int64_t)sizeof(struct dhcp_header)) return;
    void *end = &buffer[read];
    struct dhcp_header *header = (void *)&buffer[0];

    // Make sure the packet is for us.
    if (mem_compare(&header->clientHwAddr, &dhcpClient.ifreq.ifr_addr[2], 6) != 0) return;

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
                    .identifier = hc_BSWAP32(dhcpClient.currentIdentifier),
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
            hc_MEMCPY(&requestMsg.hdr.clientHwAddr, &dhcpClient.ifreq.ifr_addr[2], 6);
            hc_MEMCPY(&requestMsg.requestedIp, &header->yourIp, sizeof(requestMsg.requestedIp));

            struct sockaddr_in destAddr = {
                .sin_family = AF_INET,
                .sin_port = hc_BSWAP16(67),
                .sin_addr = { 255, 255, 255, 255 }
            };
            debug_CHECK(sys_sendto(dhcpClient.fd, &requestMsg, sizeof(requestMsg), MSG_NOSIGNAL, &destAddr, sizeof(destAddr)), RES == sizeof(requestMsg));

            // Set timeout to 10 seconds.
            struct itimerspec timeout = { .it_value = { .tv_sec = 10 } };
            CHECK(sys_timerfd_settime(dhcpClient.timerFd, 0, &timeout, NULL), RES == 0);
            break;
        }
        case dhcp_NAK: {
            // Something didn't succeed, retry in a few seconds.
            struct itimerspec timeout = { .it_value = { .tv_sec = 5 } };
            CHECK(sys_timerfd_settime(dhcpClient.timerFd, 0, &timeout, NULL), RES == 0);
            break;
        }
        case dhcp_ACK: {
            if (subnetMask == NULL || router == NULL || leaseTime == NULL) return;
            if (dns != NULL) {
                hc_MEMCPY(&dhcpClient.dnsIp, &dns[1], 4);
            } else dhcpClient.dnsIp = 0;

            uint32_t leaseTimeValue;
            hc_MEMCPY(&leaseTimeValue, &leaseTime[1], 4);
            leaseTimeValue = hc_BSWAP32(leaseTimeValue);

            // Set timeout to half of the lease time, rounded up.
            struct itimerspec timeout = { .it_value = { .tv_sec = (leaseTimeValue + 1) / 2 } };
            CHECK(sys_timerfd_settime(dhcpClient.timerFd, 0, &timeout, NULL), RES == 0);

            uint32_t netmask;
            hc_MEMCPY(&netmask, &subnetMask[1], 4);
            dhcpClient.leasedIpNetmask = hc_POPCOUNT32(netmask);

            // Record new IP lease and renew-server.
            hc_MEMCPY(&dhcpClient.renewServerIp, &header->serverIp, 4);
            if (mem_compare(&dhcpClient.leasedIp, &header->yourIp[0], 4) != 0) {
                hc_MEMCPY(&dhcpClient.leasedIp, &header->yourIp[0], 4);

                char printBuffer[18];
                char *pos = util_intToStr(&printBuffer[18], dhcpClient.leasedIpNetmask);
                *--pos = '/';
                pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[3]);
                *--pos = '.';
                pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[2]);
                *--pos = '.';
                pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[1]);
                *--pos = '.';
                pos = util_intToStr(pos, ((uint8_t *)&dhcpClient.leasedIp)[0]);

                struct iovec_const print[] = {
                    { hc_STR_COMMA_LEN("New IP leased: ") },
                    { pos, (int64_t)(&printBuffer[18] - pos) },
                    { hc_STR_COMMA_LEN("\n") }
                };
                sys_writev(2, &print[0], hc_ARRAY_LEN(print));
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
                        .ifa_prefixlen = (uint8_t)dhcpClient.leasedIpNetmask,
                        .ifa_index = dhcp_IFINDEX
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
                    }
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

static void dhcpClient_deinit(void) {
    debug_CHECK(sys_close(dhcpClient.timerFd), RES == 0);
    debug_CHECK(sys_close(dhcpClient.fd), RES == 0);
}

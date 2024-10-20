#define config_IFTYPE_WIREGUARD "wireguard\0\0"
#define config_IFTYPE_BRIDGE "bridge\0"

#define config_WAN_IF_NAME "wan"
#define config_WAN_IF_INDEX 100
#define config_LAN_IF_NAME "lan"
#define config_LAN_IF_INDEX 101

#define config_WG_IF_NAME "wg0"
#define config_WG_IF_INDEX 123
// Try to use a port that is unlikely to be blocked by firewalls.
#define config_WG_LISTEN_PORT 123
// Generate with `echo <base64-public-key> | base64 --decode | xxd -i -c 256`
#define config_WG_PEER1_PUBLIC_KEY { 0xb4, 0x9d, 0x1d, 0xd5, 0x50, 0x61, 0x5a, 0xa8, 0xf0, 0x49, 0xd0, 0x78, 0x66, 0x70, 0x09, 0xfc, 0x92, 0x56, 0xd2, 0xfa, 0x0c, 0x18, 0x29, 0x22, 0xa7, 0x7f, 0x5e, 0xff, 0x21, 0x71, 0x36, 0x5d }

struct config {
    int32_t rtnetlinkFd;
    uint16_t wgFamilyId;
    uint16_t __pad;
    char wgPublicKey[32]; // Populated by config_configure().
};

static struct config config;

static void config_init(void) {
    config.rtnetlinkFd = sys_socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    CHECK(config.rtnetlinkFd, RES > 0);

    genetlink_requestFamily(hc_STR_COMMA_LEN(WG_GENL_NAME));
    struct nlattr *wgFamilyId = genetlink_findAttr(CTRL_ATTR_FAMILY_ID);
    config.wgFamilyId = *(uint16_t *)&wgFamilyId[1];
}

static void config_addIpv4(uint8_t ifIndex, uint8_t *address, uint8_t prefixLen) {
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
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE,
        },
        .addrMsg = {
            .ifa_family = AF_INET,
            .ifa_prefixlen = prefixLen,
            .ifa_index = ifIndex
        },
        .addrAttr = {
            .nla_len = sizeof(request.addrAttr) + sizeof(request.address),
            .nla_type = IFA_LOCAL
        }
    };
    hc_MEMCPY(&request.address, address, sizeof(request.address));
    struct iovec_const iov[] = { { &request, sizeof(request) } };
    netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
}

static void config_setMaster(int32_t ifIndex, int32_t masterIfIndex, uint32_t flags, uint32_t flagsMask) {
    struct linkRequest {
        struct nlmsghdr hdr;
        struct ifinfomsg ifInfo;
        struct nlattr masterAttr;
        int32_t master;
    };
    struct linkRequest request = {
        .hdr = {
            .nlmsg_len = sizeof(request),
            .nlmsg_type = RTM_NEWLINK,
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
        },
        .ifInfo = {
            .ifi_family = AF_UNSPEC,
            .ifi_type = 0,
            .ifi_index = ifIndex,
            .ifi_flags = flags,
            .ifi_change = flagsMask
        },
        .masterAttr = {
            .nla_len = sizeof(request.masterAttr) + sizeof(request.master),
            .nla_type = IFLA_MASTER
        },
        .master = masterIfIndex
    };
    struct iovec_const iov[] = { { &request, sizeof(request) } };
    netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
}

static void config_addWgRoute(void) {
    struct addRouteRequest {
        struct nlmsghdr hdr;
        struct rtmsg rtmsg;
        struct nlattr destAttr;
        uint8_t dest[4];
        struct nlattr outIfAttr;
        uint32_t outIfIndex;
    };
    struct addRouteRequest request = {
        .hdr = {
            .nlmsg_len = sizeof(request),
            .nlmsg_type = RTM_NEWROUTE,
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE
        },
        .rtmsg = {
            .rtm_family = AF_INET,
            .rtm_dst_len = 24,
            .rtm_src_len = 0,
            .rtm_tos = 0,
            .rtm_table = RT_TABLE_MAIN,
            .rtm_protocol = RTPROT_BOOT,
            .rtm_scope = RT_SCOPE_LINK,
            .rtm_type = RTN_UNICAST,
            .rtm_flags = 0
        },
        .destAttr = {
            .nla_len = sizeof(request.destAttr) + sizeof(request.dest),
            .nla_type = RTA_DST
        },
        .dest = { 10, 123, 1, 0 },
        .outIfAttr = {
            .nla_len = sizeof(request.outIfAttr) + sizeof(request.outIfIndex),
            .nla_type = RTA_OIF
        },
        .outIfIndex = config_WG_IF_INDEX
    };
    struct iovec_const iov[] = { { &request, sizeof(request) } };
    netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
}

// Name/type sizes must be multiples of 4 (pad strings with zeroes).
static void config_addIf(int32_t ifIndex, char *ifName, uint32_t ifNameSize, char *ifType, uint32_t ifTypeSize, uint32_t flags, uint32_t flagsMask) {
    debug_ASSERT((ifNameSize & 3) == 0 && (ifTypeSize & 3) == 0);
    struct baseRequest {
        struct nlmsghdr hdr;
        struct ifinfomsg ifInfo;
        struct nlattr ifNameAttr;
    };
    struct linkInfoHdr {
        struct nlattr linkInfoAttr;
        struct nlattr linkInfoKindAttr;
    };

    struct linkInfoHdr linkInfoHdr = {
        .linkInfoAttr = {
            .nla_len = (uint16_t)(sizeof(linkInfoHdr.linkInfoAttr) + sizeof(linkInfoHdr.linkInfoKindAttr) + ifTypeSize),
            .nla_type = IFLA_LINKINFO | NLA_F_NESTED
        },
        .linkInfoKindAttr = {
            .nla_len = (uint16_t)(sizeof(linkInfoHdr.linkInfoKindAttr) + ifTypeSize),
            .nla_type = IFLA_INFO_KIND
        }
    };
    struct baseRequest baseRequest = {
        .hdr = {
            .nlmsg_len = sizeof(baseRequest) + ifNameSize + sizeof(linkInfoHdr) + ifTypeSize,
            .nlmsg_type = RTM_NEWLINK,
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE,
        },
        .ifInfo = {
            .ifi_family = AF_UNSPEC,
            .ifi_type = 0,
            .ifi_index = ifIndex,
            .ifi_flags = flags,
            .ifi_change = flagsMask
        },
        .ifNameAttr = {
            .nla_len = (uint16_t)(sizeof(baseRequest.ifNameAttr) + ifNameSize),
            .nla_type = IFLA_IFNAME
        }
    };
    struct iovec_const iov[] = {
        { &baseRequest, sizeof(baseRequest) },
        { &ifName[0],   ifNameSize },
        { &linkInfoHdr, sizeof(linkInfoHdr) },
        { &ifType[0],   ifTypeSize }
    };
    netlink_talk(config.rtnetlinkFd, &iov[0], hc_ARRAY_LEN(iov));
}

static void config_printWgPublicKey(void) {
    struct getDeviceRequest {
        struct nlmsghdr hdr;
        struct genlmsghdr genHdr;
        struct nlattr ifIndexAttr;
        uint32_t ifIndex;
    };
    struct getDeviceRequest request = {
        .hdr = {
            .nlmsg_len = sizeof(request),
            .nlmsg_type = config.wgFamilyId,
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP,
            .nlmsg_seq = 1
        },
        .genHdr = {
            .cmd = WG_CMD_GET_DEVICE,
            .version = WG_GENL_VERSION
        },
        .ifIndexAttr = {
            .nla_len = sizeof(request.ifIndexAttr) + sizeof(request.ifIndex),
            .nla_type = WGDEVICE_A_IFINDEX
        },
        .ifIndex = config_WG_IF_INDEX
    };
    struct iovec_const iov[] = { { &request, sizeof(request) } };
    genetlink_talk(&iov[0], hc_ARRAY_LEN(iov));

    struct nlattr *wgPublicKey = genetlink_findAttr(WGDEVICE_A_PUBLIC_KEY);
    hc_MEMCPY(config.wgPublicKey, (uint8_t *)&wgPublicKey[1], 32);

    char base64PublicKey[base64_ENCODE_SIZE(32)];
    base64_encode(&base64PublicKey[0], config.wgPublicKey, 32);
    #define config_PRINT_WG_PK_STR "Wireguard PK: "
    struct iovec_const print[] = {
        { hc_STR_COMMA_LEN(config_PRINT_WG_PK_STR) },
        { &base64PublicKey[0], sizeof(base64PublicKey) },
        { hc_STR_COMMA_LEN("\n") }
    };
    int64_t written = sys_writev(2, &print[0], hc_ARRAY_LEN(print));
    CHECK(written, RES == hc_STR_LEN(config_PRINT_WG_PK_STR) + sizeof(base64PublicKey) + hc_STR_LEN("\n"));
}

static void config_setWgDevice(void) {
    struct setDeviceRequest {
        struct nlmsghdr hdr;
        struct genlmsghdr genHdr;
        struct nlattr ifIndexAttr;
        uint32_t ifIndex;
        struct nlattr privateKeyAttr;
        uint8_t privateKey[32];
        struct nlattr listenPortAttr;
        uint16_t listenPort;
        char __pad[2];
        struct nlattr peersAttr;
        // Peer1
        struct nlattr peer1Attr;
        struct nlattr peer1PublicKeyAttr;
        uint8_t peer1PublicKey[32];
        struct nlattr peer1AllowedIpsAttr;
        struct nlattr peer1AllowedIpAttr;
        struct nlattr peer1AllowedIpFamilyAttr;
        uint16_t peer1AllowedIpFamily;
        char __pad2[2];
        struct nlattr peer1AllowedIpAddressAttr;
        uint8_t peer1AllowedIpAddress[4];
        struct nlattr peer1AllowedIpNetmaskAttr;
        uint8_t peer1AllowedIpNetmask;
        char __pad3[3];
        uint32_t peer1End[]; // For use with offsetof()
    };
    struct setDeviceRequest request = {
        .hdr = {
            .nlmsg_len = sizeof(request),
            .nlmsg_type = config.wgFamilyId,
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
        },
        .genHdr = {
            .cmd = WG_CMD_SET_DEVICE,
            .version = WG_GENL_VERSION
        },
        .ifIndexAttr = {
            .nla_len = sizeof(request.ifIndexAttr) + sizeof(request.ifIndex),
            .nla_type = WGDEVICE_A_IFINDEX
        },
        .ifIndex = config_WG_IF_INDEX,
        .privateKeyAttr = {
            .nla_len = sizeof(request.privateKeyAttr) + sizeof(request.privateKey),
            .nla_type = WGDEVICE_A_PRIVATE_KEY
        },
        .listenPortAttr = {
            .nla_len = sizeof(request.listenPortAttr) + sizeof(request.listenPort),
            .nla_type = WGDEVICE_A_LISTEN_PORT
        },
        .listenPort = config_WG_LISTEN_PORT,
        .peersAttr = {
            .nla_len = offsetof(struct setDeviceRequest, peer1End) - offsetof(struct setDeviceRequest, peersAttr),
            .nla_type = WGDEVICE_A_PEERS | NLA_F_NESTED
        },
        .peer1Attr = {
            .nla_len = offsetof(struct setDeviceRequest, peer1End) - offsetof(struct setDeviceRequest, peer1Attr),
            .nla_type = NLA_F_NESTED
        },
        .peer1PublicKeyAttr = {
            .nla_len = sizeof(request.peer1PublicKeyAttr) + sizeof(request.peer1PublicKey),
            .nla_type = WGPEER_A_PUBLIC_KEY
        },
        .peer1PublicKey = config_WG_PEER1_PUBLIC_KEY,
        .peer1AllowedIpsAttr = {
            .nla_len = offsetof(struct setDeviceRequest, peer1End) - offsetof(struct setDeviceRequest, peer1AllowedIpsAttr),
            .nla_type = WGPEER_A_ALLOWEDIPS | NLA_F_NESTED
        },
        .peer1AllowedIpAttr = {
            .nla_len = offsetof(struct setDeviceRequest, peer1End) - offsetof(struct setDeviceRequest, peer1AllowedIpAttr),
            .nla_type = NLA_F_NESTED
        },
        .peer1AllowedIpFamilyAttr = {
            .nla_len = sizeof(request.peer1AllowedIpFamilyAttr) + sizeof(request.peer1AllowedIpFamily),
            .nla_type = WGALLOWEDIP_A_FAMILY
        },
        .peer1AllowedIpFamily = AF_INET,
        .peer1AllowedIpAddressAttr = {
            .nla_len = sizeof(request.peer1AllowedIpAddressAttr) + sizeof(request.peer1AllowedIpAddress),
            .nla_type = WGALLOWEDIP_A_IPADDR
        },
        .peer1AllowedIpAddress = { 10, 123, 1, 1 },
        .peer1AllowedIpNetmaskAttr = {
            .nla_len = sizeof(request.peer1AllowedIpNetmaskAttr) + sizeof(request.peer1AllowedIpNetmask),
            .nla_type = WGALLOWEDIP_A_CIDR_MASK
        },
        .peer1AllowedIpNetmask = 32
    };
    // Read private key.
    int32_t fd = sys_openat(-1, "/disk/config/wg/key", O_RDONLY, 0);
    if (fd == -ENOENT) return;
    CHECK(fd, RES > 0);
    CHECK(sys_read(fd, &request.privateKey, sizeof(request.privateKey)), RES == sizeof(request.privateKey));
    debug_CHECK(sys_close(fd), RES == 0);
    struct iovec_const iov[] = { { &request, sizeof(request) } };
    genetlink_talk(&iov[0], hc_ARRAY_LEN(iov));

    config_printWgPublicKey();
}

static void config_configure(void) {
    // Don't respond to ARP on the wrong interface.
    int32_t fd = sys_openat(-1, "/proc/sys/net/ipv4/conf/all/arp_ignore", O_WRONLY, 0);
    CHECK(fd, RES > 0);
    CHECK(sys_write(fd, hc_STR_COMMA_LEN("1")), RES == 1);
    debug_CHECK(sys_close(fd), RES == 0);

    // wan
    config_addIf(
        config_WAN_IF_INDEX,
        config_WAN_IF_NAME, sizeof(config_WAN_IF_NAME),
        config_IFTYPE_BRIDGE, sizeof(config_IFTYPE_BRIDGE),
        IFF_UP, IFF_UP
    );
    // eth0
    config_setMaster(2, config_WAN_IF_INDEX, IFF_UP, IFF_UP);
    // eth1
    config_setMaster(3, config_WAN_IF_INDEX, IFF_UP, IFF_UP);
    // eth2
    config_setMaster(4, config_WAN_IF_INDEX, IFF_UP, IFF_UP);
    // eth3
    config_setMaster(5, config_WAN_IF_INDEX, IFF_UP, IFF_UP);

    // lan
    config_addIf(
        config_LAN_IF_INDEX,
        config_LAN_IF_NAME, sizeof(config_LAN_IF_NAME),
        config_IFTYPE_BRIDGE, sizeof(config_IFTYPE_BRIDGE),
        IFF_UP, IFF_UP
    );
    uint8_t lanAddress[] = { 10, 123, 0, 1 };
    config_addIpv4(config_LAN_IF_INDEX, &lanAddress[0], 24);

    // eth4
    config_setMaster(6, config_LAN_IF_INDEX, IFF_UP, IFF_UP);
    // eth5
    config_setMaster(7, config_LAN_IF_INDEX, IFF_UP, IFF_UP);
    // eth6
    config_setMaster(8, config_LAN_IF_INDEX, IFF_UP, IFF_UP);
    // eth7
    config_setMaster(9, config_LAN_IF_INDEX, IFF_UP, IFF_UP);

    // wg0
    config_addIf(
        config_WG_IF_INDEX,
        config_WG_IF_NAME, sizeof(config_WG_IF_NAME),
        config_IFTYPE_WIREGUARD, sizeof(config_IFTYPE_WIREGUARD),
        IFF_UP, IFF_UP
    );
    config_addWgRoute();
    config_setWgDevice();

    // Enable routing.
    fd = sys_openat(-1, "/proc/sys/net/ipv4/ip_forward", O_WRONLY, 0);
    CHECK(fd, RES > 0);
    CHECK(sys_write(fd, hc_STR_COMMA_LEN("1")), RES == 1);
    debug_CHECK(sys_close(fd), RES == 0);
}

static void config_deinit(void) {
    debug_CHECK(sys_close(config.rtnetlinkFd), RES == 0);
}

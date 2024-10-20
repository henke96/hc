struct genetlink {
    int32_t fd;
};

static struct genetlink genetlink;

static void genetlink_init(void) {
    genetlink.fd = sys_socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    CHECK(genetlink.fd, RES > 0);
}

static hc_INLINE void genetlink_talk(struct iovec_const *request, int32_t iovLen) {
    netlink_talk(genetlink.fd, request, iovLen);
}

static struct nlattr *genetlink_findAttr(uint16_t attrType) {
    struct nlmsghdr *msgHdr = (void *)&buffer[0];
    struct nlattr *start = (void *)&buffer[sizeof(*msgHdr) + sizeof(struct genlmsghdr)];
    void *end = &buffer[msgHdr->nlmsg_len];
    return netlink_findAttr(start, end, attrType);
}

static void genetlink_requestFamily(const char *nameZ, int32_t nameLen) {
    int32_t nameSize = nameLen + 1; // Include null terminator.
    int32_t namePadSize = math_PAD_BYTES(nameSize, 4);

    struct getFamilyRequest {
        struct nlmsghdr hdr;
        struct genlmsghdr genHdr;
        struct nlattr familyNameAttr;
    };
    int32_t requestLen = (int32_t)sizeof(struct getFamilyRequest) + nameSize + namePadSize;
    debug_ASSERT(requestLen <= UINT16_MAX);
    struct getFamilyRequest request = {
        .hdr = {
            .nlmsg_len = (uint16_t)requestLen,
            .nlmsg_type = GENL_ID_CTRL,
            .nlmsg_flags = NLM_F_REQUEST,
        },
        .genHdr = {
            .cmd = CTRL_CMD_GETFAMILY,
            .version = 1
        },
        .familyNameAttr = {
            .nla_len = sizeof(request.familyNameAttr) + (uint16_t)nameSize,
            .nla_type = CTRL_ATTR_FAMILY_NAME
        },
    };
    struct iovec_const iov[] = {
        { &request, sizeof(request) },
        { nameZ, nameSize },
        { &request, namePadSize }
    };
    genetlink_talk(&iov[0], hc_ARRAY_LEN(iov));
}

static void genetlink_deinit(void) {
    debug_CHECK(sys_close(genetlink.fd), RES == 0);
}

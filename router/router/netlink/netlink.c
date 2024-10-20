static void netlink_receive(int32_t fd) {
    // Receive full response into buffer.
    int64_t recvOffset = 0;
    for (;;) {
        int64_t remainingBuffer = (int64_t)sizeof(buffer) - recvOffset;
        CHECK(remainingBuffer, RES >= 8192); // Avoid truncations.
        int64_t received = sys_recvfrom(fd, &buffer[recvOffset], remainingBuffer, MSG_DONTWAIT, NULL, NULL);
        CHECK(received, RES > (int64_t)sizeof(struct nlmsghdr));

        // Iterate all received messages.
        void *end = (void *)&buffer[recvOffset + received];
        for (struct nlmsghdr *respHdr = (void *)&buffer[recvOffset];;) {
            if (respHdr->nlmsg_type == NLMSG_ERROR || respHdr->nlmsg_type == NLMSG_DONE) {
                int32_t error = *(int32_t *)&respHdr[1];
                CHECK(error, RES == 0);
                return;
            }
            void *next = (void *)respHdr + math_ALIGN_FORWARD(respHdr->nlmsg_len, (uint32_t)4);
            if (next == end) return;
            debug_ASSERT(next < end);
            debug_ASSERT(respHdr->nlmsg_flags & NLM_F_MULTI);
            respHdr = next;
        }
        recvOffset += received;
    }
}

static void netlink_talk(int32_t fd, struct iovec_const *request, int32_t iovLen) {
    const struct nlmsghdr *reqHdr = (const void *)request[0].iov_base;
    debug_ASSERT(request[0].iov_len >= (int64_t)sizeof(*reqHdr));

#if hc_DEBUG
    // Check that the header length matches the total iov length.
    int64_t totalLen = 0;
    for (int32_t i = 0; i < iovLen; ++i) totalLen += request[i].iov_len;
    CHECK(reqHdr->nlmsg_len, RES == totalLen);
#endif

    struct sockaddr_nl addr = { .nl_family = AF_NETLINK };
    struct msghdr_const msghdr = {
        .msg_name = &addr,
        .msg_namelen = sizeof(addr),
        .msg_iov = &request[0],
        .msg_iovlen = iovLen
    };
    int64_t sent = sys_sendmsg(fd, &msghdr, MSG_NOSIGNAL | MSG_DONTWAIT);
    CHECK(sent, RES == reqHdr->nlmsg_len);

    netlink_receive(fd);
}

static struct nlattr *netlink_findAttr(struct nlattr *start, const void *end, uint16_t attrType) {
    for (;;) {
        void *next = (void *)start + math_ALIGN_FORWARD(start->nla_len, 4);
        CHECK(next, RES <= end);

        if (start->nla_type == attrType) return start;
        start = next;
    }
}

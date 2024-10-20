struct ksmb {
    int32_t netlinkFd;
    uint16_t familyId;
    int16_t __pad;
};

static struct ksmb ksmb;

static void ksmb_init(void) {
    genetlink_requestFamily(hc_STR_COMMA_LEN(KSMBD_GENL_NAME));
    struct nlattr *familyId = genetlink_findAttr(CTRL_ATTR_FAMILY_ID);
    ksmb.familyId = *(uint16_t *)&familyId[1];

    ksmb.netlinkFd = sys_socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    CHECK(ksmb.netlinkFd, RES > 0);

    struct {
        struct nlmsghdr hdr;
        struct genlmsghdr genHdr;
        struct nlattr requestAttr;
        struct ksmbd_startup_request request;
    } startupRequest = {
        .hdr = {
            .nlmsg_len = sizeof(startupRequest),
            .nlmsg_type = ksmb.familyId,
            .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
        },
        .genHdr = {
            .cmd = KSMBD_EVENT_STARTING_UP,
            .version = KSMBD_GENL_VERSION
        },
        .requestAttr = {
            .nla_type = KSMBD_EVENT_STARTING_UP,
            .nla_len = sizeof(startupRequest.requestAttr) + sizeof(startupRequest.request)
        },
        .request = {
            .flags = 0,
            .signing = KSMBD_CONFIG_OPT_AUTO,
            .min_prot = {0},
            .max_prot = {0},
            .netbios_name = "ROUTER",
            .work_group = "WORKGROUP",
            .server_string = "ROUTER",
            .tcp_port = 445,
            .ipc_timeout = 0,
            .deadtime = 0,
            .file_max = 8192,
            .smb2_max_write = 0,
            .smb2_max_read = 0,
            .smb2_max_trans = 0,
            .share_fake_fscaps = 64,
            .sub_auth = {0},
            .smb2_max_credits = 0,
            .smbd_max_io_size = 0,
            .max_connections = 0,
            .ifc_list_sz = 0
        }
    };
    struct iovec_const iov[] = { { &startupRequest, sizeof(startupRequest) } };
    netlink_talk(ksmb.netlinkFd, &iov[0], hc_ARRAY_LEN(iov));
}

static void ksmb_onNetlinkFd(void) {
    netlink_receive(ksmb.netlinkFd);
    struct nlmsghdr *hdr = (void *)&buffer[0];
    struct genlmsghdr *genHdr = (void *)&hdr[1];
    struct nlattr *attr = (void *)&genHdr[1];
    debug_ASSERT(attr->nla_type == genHdr->cmd);

    switch (attr->nla_type) {
        case KSMBD_EVENT_LOGIN_REQUEST: {
            struct ksmbd_login_request *request = (void *)&attr[1];

            struct {
                struct nlmsghdr hdr;
                struct genlmsghdr genHdr;
                struct nlattr responseAttr;
                struct ksmbd_login_response response;
            } loginResponse = {
                .hdr = {
                    .nlmsg_len = sizeof(loginResponse),
                    .nlmsg_type = ksmb.familyId,
                    .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
                },
                .genHdr = {
                    .cmd = KSMBD_EVENT_LOGIN_RESPONSE,
                    .version = KSMBD_GENL_VERSION
                },
                .responseAttr = {
                    .nla_type = KSMBD_EVENT_LOGIN_RESPONSE,
                    .nla_len = sizeof(loginResponse.responseAttr) + sizeof(loginResponse.response)
                },
                .response = {
                    .handle = request->handle,
                    .gid = 0,
                    .uid = 0,
                    .account = {0},
                    .status = KSMBD_USER_FLAG_OK | KSMBD_USER_FLAG_GUEST_ACCOUNT,
                    .hash_sz = 16,
                    .hash = {0}
                }
            };
            hc_MEMCPY(&loginResponse.response.account[0], &request->account[0], KSMBD_REQ_MAX_ACCOUNT_NAME_SZ);
            struct iovec_const iov[] = { { &loginResponse, sizeof(loginResponse) } };
            netlink_talk(ksmb.netlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            break;
        }
        case KSMBD_EVENT_TREE_CONNECT_REQUEST: {
            struct ksmbd_tree_connect_request *request = (void *)&attr[1];

            uint16_t status = KSMBD_TREE_CONN_STATUS_NO_SHARE;
            if (
                (util_cstrCmp(&request->share[0], "config") == 0 && sys_faccessat(-1, "/disk/config", 0) == 0) ||
                (util_cstrCmp(&request->share[0], "data") == 0 && sys_faccessat(-1, "/disk/data", 0) == 0)
            ) status = KSMBD_TREE_CONN_STATUS_OK;

            struct {
                struct nlmsghdr hdr;
                struct genlmsghdr genHdr;
                struct nlattr responseAttr;
                struct ksmbd_tree_connect_response response;
            } treeResponse = {
                .hdr = {
                    .nlmsg_len = sizeof(treeResponse),
                    .nlmsg_type = ksmb.familyId,
                    .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
                },
                .genHdr = {
                    .cmd = KSMBD_EVENT_TREE_CONNECT_RESPONSE,
                    .version = KSMBD_GENL_VERSION
                },
                .responseAttr = {
                    .nla_type = KSMBD_EVENT_TREE_CONNECT_RESPONSE,
                    .nla_len = sizeof(treeResponse.responseAttr) + sizeof(treeResponse.response)
                },
                .response = {
                    .handle = request->handle,
                    .status = status,
                    .connection_flags = KSMBD_TREE_CONN_FLAG_WRITABLE
                }
            };
            struct iovec_const iov[] = { { &treeResponse, sizeof(treeResponse) } };
            netlink_talk(ksmb.netlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            break;
        }
        case KSMBD_EVENT_SHARE_CONFIG_REQUEST: {
            struct ksmbd_share_config_request *request = (void *)&attr[1];
            struct {
                struct nlmsghdr hdr;
                struct genlmsghdr genHdr;
                struct nlattr responseAttr;
                struct ksmbd_share_config_response response;
                char path[16];
            } shareResponse = {
                .hdr = {
                    .nlmsg_len = sizeof(shareResponse),
                    .nlmsg_type = ksmb.familyId,
                    .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
                },
                .genHdr = {
                    .cmd = KSMBD_EVENT_SHARE_CONFIG_RESPONSE,
                    .version = KSMBD_GENL_VERSION
                },
                .responseAttr = {
                    .nla_type = KSMBD_EVENT_SHARE_CONFIG_RESPONSE,
                    .nla_len = sizeof(shareResponse.responseAttr) + sizeof(shareResponse.response) + sizeof(shareResponse.path)
                },
                .response = {
                    .handle = request->handle,
                    .flags = 0,
                    .create_mask = 0744,
                    .directory_mask = 0755,
                    .force_create_mode = 0,
                    .force_directory_mode = 0,
                    .force_uid = (uint16_t)-1,
                    .force_gid = (uint16_t)-1,
                    .veto_list_sz = 0,
                },
            };
            if (util_cstrCmp(&request->share_name[0], "config") == 0) {
                hc_MEMCPY(&shareResponse.path[0], hc_STR_COMMA_LEN("/disk/config"));
                shareResponse.response.flags = KSMBD_SHARE_FLAG_AVAILABLE | KSMBD_SHARE_FLAG_BROWSEABLE | KSMBD_SHARE_FLAG_WRITEABLE;
            } else if (util_cstrCmp(&request->share_name[0], "data") == 0) {
                hc_MEMCPY(&shareResponse.path[0], hc_STR_COMMA_LEN("/disk/data"));
                shareResponse.response.flags = KSMBD_SHARE_FLAG_AVAILABLE | KSMBD_SHARE_FLAG_BROWSEABLE | KSMBD_SHARE_FLAG_WRITEABLE;
            }
            struct iovec_const iov[] = { { &shareResponse, sizeof(shareResponse) } };
            netlink_talk(ksmb.netlinkFd, &iov[0], hc_ARRAY_LEN(iov));
            break;
        }
        default: break;
    }
}

static void ksmb_deinit(void) {
    debug_CHECK(sys_close(ksmb.netlinkFd), RES == 0);

    // Attempt to stop server.
    int32_t killServerFd = sys_openat(-1, "/sys/class/ksmbd-control/kill_server", O_WRONLY, 0);
    debug_ASSERT(killServerFd >= 0);
    if (killServerFd >= 0) {
        debug_CHECK(sys_write(killServerFd, hc_STR_COMMA_LEN("hard")), RES == 4);
        debug_CHECK(sys_close(killServerFd), RES == 0);
    }
}

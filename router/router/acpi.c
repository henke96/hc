struct acpi {
    int32_t netlinkFd;
    uint16_t mcastGrpId;
    int16_t __pad;
};

static struct acpi acpi;

static void acpi_init(void) {
    genetlink_requestFamily(hc_STR_COMMA_LEN(ACPI_EVENT_FAMILY_NAME));
    struct nlattr *groupsAttr = genetlink_findAttr(CTRL_ATTR_MCAST_GROUPS);

    for (uint16_t groupNum = 1;; ++groupNum) {
        struct nlattr *groupAttr = netlink_findAttr(&groupsAttr[1], (void *)groupsAttr + groupsAttr->nla_len, groupNum);
        struct nlattr *groupNameAttr = netlink_findAttr(&groupAttr[1], (void *)groupAttr + groupAttr->nla_len, CTRL_ATTR_MCAST_GRP_NAME);

        if (mem_compare(&groupNameAttr[1], ACPI_EVENT_MCAST_GROUP_NAME, sizeof(ACPI_EVENT_MCAST_GROUP_NAME)) == 0) {
            struct nlattr *groupIdAttr = netlink_findAttr(&groupAttr[1], (void *)groupAttr + groupAttr->nla_len, CTRL_ATTR_MCAST_GRP_ID);
            acpi.mcastGrpId = *(uint16_t *)&groupIdAttr[1];
            break;
        }
    }

    acpi.netlinkFd = sys_socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    CHECK(acpi.netlinkFd, RES > 0);

    // Bind the mcast group.
    struct sockaddr_nl addr = {
        .nl_family = AF_NETLINK,
        .nl_groups = 1 << (acpi.mcastGrpId - 1)
    };
    CHECK(sys_bind(acpi.netlinkFd, &addr, sizeof(addr)), RES == 0);
}

static void acpi_deinit(void) {
    debug_CHECK(sys_close(acpi.netlinkFd), RES == 0);
}

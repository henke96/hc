
static void iptables_configure(void) {
    int32_t fd = sys_socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    CHECK(fd, RES > 0);

    struct acceptEntry {
        struct ipt_entry entry;
        struct xt_standard_target standardTarget;
    };
    struct replace {
        struct ipt_replace iptReplace;
        struct {
            struct acceptEntry preroutingEntry;
            struct acceptEntry inputEntry;
            struct acceptEntry outputEntry;
            struct {
                struct ipt_entry entry;
                struct {
                    struct xt_entry_target target;
                    struct nf_nat_ipv4_multi_range_compat data;
                    int32_t __pad;
                } natTarget;
            } postroutingNatEntry;
            struct acceptEntry postroutingEntry;
        } entries;
    };

    struct acceptEntry acceptEntry = {
        .entry = {
            .target_offset = sizeof(acceptEntry.entry),
            .next_offset = sizeof(acceptEntry)
        },
        .standardTarget = {
            .target = {
                .target_size = sizeof(acceptEntry.standardTarget),
                .name = XT_STANDARD_TARGET
            },
            .verdict = -(NF_ACCEPT) - 1
        }
    };
    struct replace replace = {
        .iptReplace = {
            .name = "nat",
            .valid_hooks = 0b11011, // All except forwarding, see enum nf_inet_hooks.
            .num_entries = 5,
            .size = sizeof(replace.entries),
            .hook_entry = {
                offsetof(struct replace, entries.preroutingEntry) - offsetof(struct replace, entries),
                offsetof(struct replace, entries.inputEntry) - offsetof(struct replace, entries),
                0,
                offsetof(struct replace, entries.outputEntry) - offsetof(struct replace, entries),
                offsetof(struct replace, entries.postroutingNatEntry) - offsetof(struct replace, entries)
            },
            .underflow = {
                offsetof(struct replace, entries.preroutingEntry) - offsetof(struct replace, entries),
                offsetof(struct replace, entries.inputEntry) - offsetof(struct replace, entries),
                0,
                offsetof(struct replace, entries.outputEntry) - offsetof(struct replace, entries),
                offsetof(struct replace, entries.postroutingEntry) - offsetof(struct replace, entries),
            },
            .num_counters = 5,
            .counters = (struct xt_counters *)&buffer[0]
        },
        .entries = {
            .preroutingEntry = acceptEntry,
            .inputEntry = acceptEntry,
            .outputEntry = acceptEntry,
            .postroutingNatEntry = {
                .entry = {
                    .ip = {
                        .src = { 10, 123, 0, 0 },
                        .smsk = { 255, 255, 0, 0 },
                        .dst = { 10, 123, 0, 0 },
                        .dmsk = { 255, 255, 0, 0 },
                        .invflags = IPT_INV_DSTIP
                    },
                    .target_offset = sizeof(replace.entries.postroutingNatEntry.entry),
                    .next_offset = sizeof(replace.entries.postroutingNatEntry)
                },
                .natTarget = {
                    .target = {
                        .target_size = sizeof(replace.entries.postroutingNatEntry.natTarget),
                        .name = "MASQUERADE"
                    },
                    .data = {
                        .rangesize = 1
                    }
                }
            },
            .postroutingEntry = acceptEntry
        }
    };

    int32_t status = sys_setsockopt(fd, SOL_IP, IPT_SO_SET_REPLACE, &replace, sizeof(replace));
    CHECK(status, RES == 0);
    debug_CHECK(sys_close(fd), RES == 0);
}

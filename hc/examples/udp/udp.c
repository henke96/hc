#include "hc/hc.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
static noreturn void abort(void) {
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
}
#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"
#include "hc/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/argParse.h"

#include "options.c"

static char buffer[65536] hc_ALIGNED(16);

static int32_t join_multicast(int32_t fd, char *groups, int32_t ifIndex) {
    struct ip_mreqn mreq = { .imr_ifindex = ifIndex };
    for (;;) {
        for (int32_t i = 0;;) {
            uint64_t octet;
            int32_t parsed = util_strToUint(groups, INT32_MAX, &octet);
            if (parsed <= 0 || octet > 255) return -1;
            mreq.imr_multiaddr[i] = (uint8_t)octet;
            groups += parsed;

            if (++i == hc_ARRAY_LEN(mreq.imr_multiaddr)) break;
            if (*groups != '.') return -1;
            groups += 1;
        }
        int32_t status = sys_setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
        if (status < 0) return -1;
        if (*groups == '\0') break;
        else if (*groups != ',') return -1;
        groups += 1;
    }
    return 0;
}

int32_t start(hc_UNUSED int32_t argc, char **argv, hc_UNUSED char **envp) {
    struct options options;
    if (options_init(&options, argv) < 0) {
        debug_print(
            "\nUsage: udp [OPTION]...\n"
            "Options:\n"
            "    -b, --bind <bind-address>\n"
            "    -p, --port <bind-port>\n"
            "    -i, --interface <bind-interface>\n"
            "    -m, --multicast-groups <groups>\n"
            "        comma-separated list of multicast groups to join\n"
        );
        return 1;
    }

    int32_t fd = sys_socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return 1;

    int32_t one = 1;
    int32_t status = sys_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if (status < 0) return 1;

    int32_t zero = 0;
    status = sys_setsockopt(fd, IPPROTO_IP, IP_MULTICAST_ALL, &zero, sizeof(zero));
    if (status < 0) return 1;

    int32_t ifIndex = 0;
    if (options.interface != NULL) {
        status = sys_setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, options.interface, options.interfaceSize);
        if (status < 0) {
            debug_print("Failed to bind device\n");
            return 1;
        }

        struct ifreq ifreq;
        hc_MEMCPY(&ifreq.ifr_name[0], options.interface, (uint64_t)options.interfaceSize);
        status = sys_ioctl(fd, SIOCGIFINDEX, &ifreq);
        if (status < 0) return 1;
        ifIndex = ifreq.ifr_ifindex;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(options.port),
    };
    hc_MEMCPY(&addr.sin_addr[0], &options.bindAddress[0], sizeof(addr.sin_addr));
    status = sys_bind(fd, &addr, sizeof(addr));
    if (status < 0) {
        debug_print("Failed to bind address\n");
        return 1;
    }

    if (options.multicastGroups != NULL) {
        status = join_multicast(fd, options.multicastGroups, ifIndex);
        if (status < 0) {
            debug_print("Failed to join multicast groups\n");
            return 1;
        }
    }

    uint64_t totalRead = 0;
    for (;;) {
        int64_t numRead = sys_recvfrom(fd, &buffer[0], sizeof(buffer), 0, NULL, NULL);
        if (numRead == 0) break;
        if (numRead < 0) {
            debug_print("Receive failed\n");
            return 1;
        }
        totalRead += (uint64_t)numRead;
        char print[hc_STR_LEN("Total received: ") + util_UINT64_MAX_CHARS + hc_STR_LEN("\n")];
        char *pos = hc_ARRAY_END(print);
        hc_PREPEND_STR(pos, "\n");
        pos = util_uintToStr(pos, totalRead);
        hc_PREPEND_STR(pos, "Total received: ");
        debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
    }
    return 0;
}

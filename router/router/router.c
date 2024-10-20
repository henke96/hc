#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/mem.c"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone3_func.c"

#define CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)

// Shared buffer space for whole program.
static char buffer[66000] hc_ALIGNED(16); // Netlink wants 8192, see NLMSG_GOODSIZE in <linux/netlink.h>.
                                          // packetDumper wants to support jumbo frames, so use 66000 to be (very) safe.
#define buffer_HALF (sizeof(buffer) / 2)

static void epollAdd(int32_t epollFd, int32_t fd) {
    int32_t status = sys_epoll_ctl(
        epollFd,
        EPOLL_CTL_ADD,
        fd,
        &(struct epoll_event) { .events = EPOLLIN, .data.fd = fd }
    );
    CHECK(status, RES == 0);
}

#include "disk.c"
#include "dhcp.h"
#include "netlink/netlink.c"
#include "netlink/genetlink.c"
#include "ksmb.c"
#include "acpi.c"
#include "config.c"
#include "dhcpClient.c"
#include "dhcpServer.c"
#include "iptables.c"
#include "packetDumper.c"
#include "modemClient.c"
#include "hostapd.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    disk_mount();

    genetlink_init();

    acpi_init();
    ksmb_init();
    config_init();

    config_configure();
    hostapd_init();

    dhcpClient_init();
    iptables_configure();

    struct dhcpServer dhcpServer;
    uint8_t lanIp[4] hc_ALIGNED(4) = { 10, 123, 0, 1 };
    dhcpServer_init(&dhcpServer, config_LAN_IF_INDEX, *(uint32_t *)&lanIp[0]);

    struct packetDumper wanDumper;
    packetDumper_init(&wanDumper, config_WAN_IF_INDEX);
    struct packetDumper lanDumper;
    packetDumper_init(&lanDumper, config_LAN_IF_INDEX);

    struct modemClient modemClient;
    modemClient_init(&modemClient, "/dev/ttyUSB2");

    // Setup epoll.
    int32_t epollFd = sys_epoll_create1(0);
    CHECK(epollFd, RES > 0);
    epollAdd(epollFd, acpi.netlinkFd);
    epollAdd(epollFd, ksmb.netlinkFd);
    if (hostapd.pidFd > 0) epollAdd(epollFd, hostapd.pidFd);
    epollAdd(epollFd, dhcpClient.fd);
    epollAdd(epollFd, dhcpClient.timerFd);
    epollAdd(epollFd, dhcpServer.fd);
    epollAdd(epollFd, wanDumper.listenFd);
    epollAdd(epollFd, lanDumper.listenFd);
    epollAdd(epollFd, modemClient.timerFd);

    for (;;) {
        struct epoll_event event;
        event.data.fd = 0;
        CHECK(sys_epoll_pwait(epollFd, &event, 1, -1, NULL), RES == 1);
        if (event.data.fd == acpi.netlinkFd) break;
        if (event.data.fd == hostapd.pidFd) debug_abort();

        if (event.data.fd == ksmb.netlinkFd) ksmb_onNetlinkFd();
        else if (event.data.fd == dhcpClient.fd) dhcpClient_onFd();
        else if (event.data.fd == dhcpClient.timerFd) dhcpClient_onTimerFd();
        else if (event.data.fd == dhcpServer.fd) dhcpServer_onFd(&dhcpServer);
        else if (event.data.fd == wanDumper.listenFd) packetDumper_onListenFd(&wanDumper, epollFd);
        else if (event.data.fd == wanDumper.packetFd) packetDumper_onPacketFd(&wanDumper);
        else if (event.data.fd == wanDumper.clientFd) packetDumper_onClientFd(&wanDumper);
        else if (event.data.fd == lanDumper.listenFd) packetDumper_onListenFd(&lanDumper, epollFd);
        else if (event.data.fd == lanDumper.packetFd) packetDumper_onPacketFd(&lanDumper);
        else if (event.data.fd == lanDumper.clientFd) packetDumper_onClientFd(&lanDumper);
        else if (event.data.fd == modemClient.timerFd) modemClient_onTimerFd(&modemClient, epollFd);
        else if (event.data.fd == modemClient.fd) modemClient_onFd(&modemClient);
        else debug_ASSERT(0);
    }

    modemClient_deinit(&modemClient);
    packetDumper_deinit(&lanDumper);
    packetDumper_deinit(&wanDumper);
    dhcpServer_deinit(&dhcpServer);
    dhcpClient_deinit();
    hostapd_deinit();
    config_deinit();
    ksmb_deinit();
    acpi_deinit();
    genetlink_deinit();
    disk_unmount();
    return 0;
}

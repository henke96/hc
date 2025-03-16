#include "hc/hc.h"
#include "hc/mem.c"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/telnet.h"
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
struct dhcpClient wanDhcpClient;
#include "dhcpServer.c"
#include "iptables.c"
#include "packetDumper.c"
#include "modemClient.c"
#include "hostapd.c"
#include "telnetServer.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    CHECK(sys_dup3(0, 3, O_CLOEXEC), RES == 3);

    disk_mount();

    genetlink_init();

    acpi_init();
    ksmb_init();
    config_init();

    iptables_configure();
    config_configure();
    hostapd_init();

    dhcpClient_init(&wanDhcpClient, config_WAN_IF_INDEX, 100);
    struct dhcpClient usbDhcpClient = {
        .fd = -1,
        .timerFd = -1   
    };
    struct packetDumper usbDumper = {
        .listenFd = -1,
        .clientFd = -1,
        .packetFd = -1
    };
    // TODO: Get rid of sleep..
    struct timespec sleep = { .tv_sec = 5 };
    CHECK(sys_clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL), RES == 0);
    int32_t usbIfIndex = config_getIfIndex("usb0");
    if (usbIfIndex >= 0) {
        config_setFlags(usbIfIndex, IFF_UP, IFF_UP);
        dhcpClient_init(&usbDhcpClient, usbIfIndex, 101);
        packetDumper_init(&usbDumper, usbIfIndex, 101);
    }

    struct packetDumper wanDumper;
    packetDumper_init(&wanDumper, config_WAN_IF_INDEX, 100);
    struct packetDumper lanDumper;
    packetDumper_init(&lanDumper, config_LAN_IF_INDEX, 102);

    struct dhcpServer dhcpServer;
    uint8_t lanIp[4] hc_ALIGNED(4) = { 10, 123, 0, 1 };
    dhcpServer_init(&dhcpServer, config_LAN_IF_INDEX, *(uint32_t *)&lanIp[0]);

    struct modemClient modemClient;
    modemClient_init(&modemClient, "/dev/ttyUSB2");

    telnetServer_init();

    // Setup epoll.
    int32_t epollFd = sys_epoll_create1(0);
    CHECK(epollFd, RES > 0);
    epollAdd(epollFd, acpi.netlinkFd);
    epollAdd(epollFd, ksmb.netlinkFd);
    if (hostapd.pidFd > 0) epollAdd(epollFd, hostapd.pidFd);
    epollAdd(epollFd, wanDhcpClient.fd);
    epollAdd(epollFd, wanDhcpClient.timerFd);
    if (usbDhcpClient.fd >= 0) {
        epollAdd(epollFd, usbDhcpClient.fd);
        epollAdd(epollFd, usbDhcpClient.timerFd);
    }
    if (usbDumper.listenFd >= 0) epollAdd(epollFd, usbDumper.listenFd);
    epollAdd(epollFd, wanDumper.listenFd);
    epollAdd(epollFd, lanDumper.listenFd);
    epollAdd(epollFd, dhcpServer.fd);
    epollAdd(epollFd, modemClient.timerFd);
    epollAdd(epollFd, telnetServer.listenFd);
    epollAdd(epollFd, telnetServer.pidFd);
    epollAdd(epollFd, telnetServer.ptMasterFd);

    for (;;) {
        struct epoll_event event;
        event.data.fd = 0;
        CHECK(sys_epoll_pwait(epollFd, &event, 1, -1, NULL), RES == 1);
        if (event.data.fd == acpi.netlinkFd) break;
        if (event.data.fd == hostapd.pidFd || event.data.fd == telnetServer.pidFd) util_abort();

        if (event.data.fd == ksmb.netlinkFd) ksmb_onNetlinkFd();
        else if (event.data.fd == wanDhcpClient.fd) dhcpClient_onFd(&wanDhcpClient);
        else if (event.data.fd == wanDhcpClient.timerFd) dhcpClient_onTimerFd(&wanDhcpClient);
        else if (event.data.fd == usbDhcpClient.fd) dhcpClient_onFd(&usbDhcpClient);
        else if (event.data.fd == usbDhcpClient.timerFd) dhcpClient_onTimerFd(&usbDhcpClient);
        else if (event.data.fd == dhcpServer.fd) dhcpServer_onFd(&dhcpServer);
        else if (event.data.fd == wanDumper.listenFd) packetDumper_onListenFd(&wanDumper, epollFd);
        else if (event.data.fd == wanDumper.packetFd) packetDumper_onPacketFd(&wanDumper);
        else if (event.data.fd == wanDumper.clientFd) packetDumper_onClientFd(&wanDumper);
        else if (event.data.fd == lanDumper.listenFd) packetDumper_onListenFd(&lanDumper, epollFd);
        else if (event.data.fd == lanDumper.packetFd) packetDumper_onPacketFd(&lanDumper);
        else if (event.data.fd == lanDumper.clientFd) packetDumper_onClientFd(&lanDumper);
        else if (event.data.fd == usbDumper.listenFd) packetDumper_onListenFd(&usbDumper, epollFd);
        else if (event.data.fd == usbDumper.packetFd) packetDumper_onPacketFd(&usbDumper);
        else if (event.data.fd == usbDumper.clientFd) packetDumper_onClientFd(&usbDumper);
        else if (event.data.fd == modemClient.timerFd) modemClient_onTimerFd(&modemClient, epollFd);
        else if (event.data.fd == modemClient.fd) modemClient_onFd(&modemClient);
        else if (event.data.fd == telnetServer.listenFd) telnetServer_onListenFd(epollFd);
        else if (event.data.fd == telnetServer.ptMasterFd) telnetServer_onPtMasterFd();
        else if (event.data.fd == telnetServer.clientFd) telnetServer_onClientFd();
        else debug_ASSERT(0);
    }

    telnetServer_deinit();
    modemClient_deinit(&modemClient);
    dhcpServer_deinit(&dhcpServer);
    packetDumper_deinit(&lanDumper);
    packetDumper_deinit(&wanDumper);
    if (usbDumper.listenFd >= 0) packetDumper_deinit(&usbDumper);
    if (usbDhcpClient.fd >= 0) dhcpClient_deinit(&usbDhcpClient);
    dhcpClient_deinit(&wanDhcpClient);
    hostapd_deinit();
    config_deinit();
    ksmb_deinit();
    acpi_deinit();
    genetlink_deinit();
    disk_unmount();
    return 0;
}

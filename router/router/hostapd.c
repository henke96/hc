struct hostapd {
    int32_t pidFd;
};

static struct hostapd hostapd;

#define hostapd_CONF "\
interface=wlan0\n\
bridge=lan\n\
driver=nl80211\n\
ssid=wifi\n\
hw_mode=g\n\
channel=1\n\
ieee80211n=1\n\
ht_capab=[HT40+][SHORT-GI-40]\n\
wpa=2\n\
wpa_key_mgmt=WPA-PSK\n\
rsn_pairwise=CCMP\n\
wpa_passphrase="

static noreturn void _hostapd(hc_UNUSED void *arg) {
    if (sys_close_range(3, INT32_MAX, CLOSE_RANGE_UNSHARE) == 0) {
        const char *argv[] = { "/hostapd", "/tmp/hostapd.conf", NULL };
        const char *envp[] = { NULL };
        sys_execveat(-1, argv[0], &argv[0], &envp[0], 0);
    }
    sys_exit_group(1);
}

static void hostapd_init(void) {
    hostapd.pidFd = INT32_MIN;

    // Read config.
    int32_t fd = sys_openat(-1, "/disk/config/wifi/password", O_RDONLY, 0);
    if (fd == -ENOENT) return;
    CHECK(fd, RES > 0);
    int64_t passwordSize = sys_read(fd, &buffer[0], sizeof(buffer));
    CHECK(passwordSize, RES >= 8 && RES <= 63);
    debug_CHECK(sys_close(fd), RES == 0);

    // Write hostapd.conf.
    fd = sys_openat(-1, "/tmp/hostapd.conf", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    CHECK(fd, RES > 0);
    CHECK(sys_write(fd, hc_STR_COMMA_LEN(hostapd_CONF)), RES == hc_STR_LEN(hostapd_CONF));
    CHECK(sys_write(fd, &buffer[0], passwordSize), RES == passwordSize);
    debug_CHECK(sys_close(fd), RES == 0);

    // Start hostapd.
    struct clone_args args = {
        .flags = CLONE_VM | CLONE_FILES | CLONE_VFORK | CLONE_PIDFD,
        .pidfd = &hostapd.pidFd,
        .stack = &buffer[0],
        .stack_size = sizeof(buffer)
    };
    CHECK(sys_clone3_func(&args, sizeof(args), _hostapd, NULL), RES > 0);
}

static void hostapd_deinit(void) {
    if (hostapd.pidFd > 0) debug_CHECK(sys_close(hostapd.pidFd), RES == 0);
}

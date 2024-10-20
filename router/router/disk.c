#define disk_EXT_MAGIC_OFFSET 0x38
#define disk_EXT_VOLUME_NAME_OFFSET 0x78

static bool _disk_findAndMount(int32_t devFd, int32_t sysClassBlockFd) {
    for (;;) {
        int64_t numRead = sys_getdents64(devFd, &buffer[0], buffer_HALF);
        if (numRead == 0) break;
        CHECK(numRead, RES > 0);
        for (
            struct linux_dirent64 *current = (void *)&buffer[0];
            current != (void *)&buffer[numRead];
            current = (void *)current + current->d_reclen
        ) {
            char *name = &current->d_name[0];
            if (name[0] == '.') continue;
            if (sys_faccessat(sysClassBlockFd, name, 0) != 0) continue;

            int32_t fd = sys_openat(devFd, name, O_RDONLY, 0);
            if (fd < 0) continue;

            // Check if this is the device we are looking for.
            char *superblock = &buffer[buffer_HALF];
            int64_t status = sys_pread64(fd, superblock, disk_EXT_VOLUME_NAME_OFFSET + 16, 1024);
            debug_CHECK(sys_close(fd), RES == 0);
            if (
                status != disk_EXT_VOLUME_NAME_OFFSET + 16 ||
                *(uint16_t *)hc_ASSUME_ALIGNED(&superblock[disk_EXT_MAGIC_OFFSET], 2) != 0xEF53 ||
                mem_compare(&superblock[disk_EXT_VOLUME_NAME_OFFSET], hc_STR_COMMA_LEN("ROUTER\0")) != 0
            ) continue;

            // Mount the device.
            int64_t nameLen = util_cstrLen(name);
            hc_MEMCPY(&buffer[buffer_HALF], hc_STR_COMMA_LEN("/dev/"));
            hc_MEMCPY(&buffer[buffer_HALF + hc_STR_LEN("/dev/")], name, (uint64_t)nameLen + 1);
            CHECK(sys_mount(&buffer[buffer_HALF], "/disk", "ext4", MS_NOATIME, NULL), RES == 0);
            #define disk_MOUNT_STR "Mounted disk "
            struct iovec_const print[] = {
                { hc_STR_COMMA_LEN(disk_MOUNT_STR) },
                { name, nameLen },
                { hc_STR_COMMA_LEN("\n") }
            };
            int64_t written = sys_writev(2, &print[0], hc_ARRAY_LEN(print));
            CHECK(written, RES == (int64_t)hc_STR_LEN(disk_MOUNT_STR) + nameLen + (int64_t)hc_STR_LEN("\n"));
            return true;
        }
    }
    CHECK(sys_lseek(devFd, 0, SEEK_SET), RES == 0);
    return false;
}

static void disk_mount(void) {
    int32_t devFd = sys_openat(-1, "/dev", O_RDONLY, 0);
    CHECK(devFd, RES > 0);
    int32_t sysClassBlockFd = sys_openat(-1, "/sys/class/block", O_RDONLY, 0);
    CHECK(sysClassBlockFd, RES > 0);

    for (;;) {
        if (_disk_findAndMount(devFd, sysClassBlockFd)) break;
        struct timespec sleep = { .tv_nsec = 100000000 };
        CHECK(sys_clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL), RES == 0);
    }

    debug_CHECK(sys_close(devFd), RES == 0);
    debug_CHECK(sys_close(sysClassBlockFd), RES == 0);
}

static void disk_unmount(void) {
    for (;;) {
        int32_t status = sys_umount2("/disk", 0);
        if (status == 0) break;
        debug_printNum("Failed to unmount /disk (", status, ")\n");
        struct timespec sleep = { .tv_sec = 1 };
        CHECK(sys_clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL), RES == 0);
    }
}

#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone3_func.c"

static char buffer[4096] hc_ALIGNED(16);

static int32_t initialise(void) {
    if (sys_mount("", "/proc", "proc", 0, NULL) < 0) return -1;

    // Disable memory overcommit.
    int32_t fd = sys_openat(-1, "/proc/sys/vm/overcommit_memory", O_WRONLY, 0);
    if (fd < 0) return -2;
    int64_t num = sys_write(fd, "2", 1);
    if (sys_close(fd) != 0) return -3;
    if (num != 1) return -4;

    // Panic if we run out of memory anyway.
    fd = sys_openat(-1, "/proc/sys/vm/panic_on_oom", O_WRONLY, 0);
    if (fd < 0) return -5;
    num = sys_write(fd, hc_STR_COMMA_LEN("2"));
    if (sys_close(fd) != 0) return -6;
    if (num != 1) return -7;

    if (sys_mount("", "/sys", "sysfs", 0, NULL) < 0) return -8;
    if (sys_mount("", "/dev", "devtmpfs", 0, NULL) < 0) return -9;
    return 0;
}

static noreturn void run(void *program) {
    const char *newArgv[] = { program, NULL };
    const char *newEnvp[] = { "HOME=/", "TERM=linux", NULL };
    sys_execveat(-1, newArgv[0], &newArgv[0], &newEnvp[0], 0);
    sys_exit_group(1);
}

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    uint32_t rebootCmd = LINUX_REBOOT_CMD_HALT;
    int32_t status = initialise();
    if (status < 0) {
        debug_printNum("Failed to initialise (", status, ")\n");
        goto halt;
    }

    struct clone_args args = {
        .flags = CLONE_VM | CLONE_VFORK,
        .exit_signal = SIGCHLD,
        .stack = &buffer[0],
        .stack_size = sizeof(buffer)
    };
    int32_t shellPid = sys_clone3_func(&args, sizeof(args), run, "/shell");
    if (shellPid < 0) goto halt;

    int32_t routerPid = sys_clone3_func(&args, sizeof(args), run, "/router");
    if (routerPid < 0) goto halt;

    // Wait for children.
    for (;;) {
        struct rusage rusage;
        rusage.ru_maxrss = 0;
        int32_t pid = sys_wait4(-1, &status, 0, &rusage);
        if (pid < 0) goto halt;

        #define PID_END util_INT32_MAX_CHARS
        #define STATUS_END (PID_END + util_INT32_MAX_CHARS)
        #define MAXRSS_END (STATUS_END + util_INT64_MAX_CHARS)
        char *pidStr = util_intToStr(&buffer[PID_END], pid);
        char *statusStr = util_intToStr(&buffer[STATUS_END], status);
        char *maxRssStr = util_intToStr(&buffer[MAXRSS_END], rusage.ru_maxrss);
        struct iovec_const iov[] = {
            { hc_STR_COMMA_LEN("Pid ") },
            { pidStr, (int64_t)(&buffer[PID_END] - pidStr) },
            { hc_STR_COMMA_LEN(" exited (status=") },
            { statusStr, (int64_t)(&buffer[STATUS_END] - statusStr) },
            { hc_STR_COMMA_LEN(", maxRss=") },
            { maxRssStr, (int64_t)(&buffer[MAXRSS_END] - maxRssStr) },
            { hc_STR_COMMA_LEN(")\n") }
        };
        sys_writev(2, &iov[0], hc_ARRAY_LEN(iov));

        if (pid == routerPid) {
            if (status == 0) rebootCmd = LINUX_REBOOT_CMD_POWER_OFF;
            goto halt;
        }
    }

    halt:
    sys_sync();
    sys_reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, rebootCmd, NULL);
    return 0;
}

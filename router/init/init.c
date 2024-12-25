#include "hc/hc.h"
#include "hc/math.c"
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
#include "hc/linux/helpers/sys_clone3_func.c"

static char buffer[4096] hc_ALIGNED(16);

static int32_t initialise(void) {
    if (sys_mount("", "/proc", "proc", 0, NULL) < 0) return -1;

    // Disable memory overcommit.
    int32_t fd = sys_openat(-1, "/proc/sys/vm/overcommit_memory", O_WRONLY, 0);
    if (fd < 0) return -2;
    int32_t status = util_writeAll(fd, hc_STR_COMMA_LEN("2"));
    if (sys_close(fd) != 0) return -3;
    if (status < 0) return -4;

    // Panic if we run out of memory anyway.
    fd = sys_openat(-1, "/proc/sys/vm/panic_on_oom", O_WRONLY, 0);
    if (fd < 0) return -5;
    status = util_writeAll(fd, hc_STR_COMMA_LEN("2"));
    if (sys_close(fd) != 0) return -6;
    if (status < 0) return -7;

    if (sys_mount("", "/sys", "sysfs", 0, NULL) < 0) return -8;
    if (sys_mount("", "/dev", "devtmpfs", 0, NULL) < 0) return -9;
    if (sys_mount("", "/devpts", "devpts", 0, "ptmxmode=0600") < 0) return -10;
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
        debug_printNum("Failed to initialise", status);
        goto halt;
    }

    struct clone_args args = {
        .flags = CLONE_VM | CLONE_VFORK,
        .stack = &buffer[0],
        .stack_size = sizeof(buffer)
    };
    int32_t shellPid = sys_clone3_func(&args, sizeof(args), run, "/shell");
    if (shellPid < 0) goto halt;

    int32_t routerPid = sys_clone3_func(&args, sizeof(args), run, "/router");
    if (routerPid < 0) goto halt;

    // Wait for children.
    for (;;) {
        struct siginfo info;
        // Make static analysis happy.
        info.si_status = 0;
        info.si_code = 0;
        struct rusage rusage;
        rusage.ru_maxrss = 0; // Make static analysis happy.
        status = sys_waitid(P_ALL, 0, &info, WEXITED | __WALL, &rusage);
        if (status < 0) goto halt;

        char print[
            hc_STR_LEN("Pid ") +
            util_INT32_MAX_CHARS +
            hc_MAX(hc_STR_LEN(" exited (status="), hc_STR_LEN(" killed (signal=")) +
            util_INT32_MAX_CHARS +
            hc_STR_LEN(", maxRss=") +
            util_INT64_MAX_CHARS +
            hc_STR_LEN(")\n")
        ];
        char *pos = hc_ARRAY_END(print);
        hc_PREPEND_STR(pos, ")\n");
        pos = util_intToStr(pos, rusage.ru_maxrss);
        hc_PREPEND_STR(pos, ", maxRss=");
        pos = util_intToStr(pos, info.si_status);
        if (info.si_code == CLD_EXITED) {
            hc_PREPEND_STR(pos, " exited (status=");
        } else {
            hc_PREPEND_STR(pos, " killed (signal=");
        }
        pos = util_intToStr(pos, info.si_pid);
        hc_PREPEND_STR(pos, "Pid ");
        if (util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos) < 0) goto halt;

        if (info.si_pid == routerPid && info.si_code == CLD_EXITED && info.si_status == 0) {
            rebootCmd = LINUX_REBOOT_CMD_POWER_OFF;
            goto halt;
        }
    }

    halt:
    sys_sync();
    sys_reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, rebootCmd, NULL);
    return 0;
}

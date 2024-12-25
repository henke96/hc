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
#include "hc/linux/helpers/sys_clone3_func.c"

static char buffer[65536] hc_ALIGNED(16);

#define PROMPT "Shell usage: program^@arg1^@arg2^D^D\n"
#define INVALID_INPUT "Invalid input\n"
#define FAILED_RUN "Failed to run program, errno = "
#define SUCCESSFUL_RUN "Program exited, status = "

static int32_t run_execErrno;
static char run_stack[4096] hc_ALIGNED(16);
static char *run_argv[256];
static noreturn void run(void *arg) {
    const char **envp = arg;
    run_execErrno = 0;
    run_execErrno = -sys_execveat(AT_FDCWD, run_argv[0], (const char *const *)&run_argv[0], &envp[0], 0);
    sys_exit_group(0);
}

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    for (;;) {
        // Promt user and read input.
        if (util_writeAll(util_STDERR, hc_STR_COMMA_LEN(PROMPT)) < 0) return 1;
        int64_t inputSize = util_readAll(0, &buffer[0], sizeof(buffer));
        if (inputSize < 0) return 1;

        if (util_writeAll(util_STDERR, hc_STR_COMMA_LEN("\n")) < 0) return 1;
        if (inputSize == sizeof(buffer)) goto invalidInput;
        buffer[inputSize++] = '\0';

        // Parse input.
        int32_t argsCount = 0;
        for (int64_t i = 0; i < inputSize;) {
            if (argsCount >= (int32_t)hc_ARRAY_LEN(run_argv) - 1) goto invalidInput;
            run_argv[argsCount++] = &buffer[i];
            while (buffer[i++] != '\0');
        }
        run_argv[argsCount] = NULL;

        // Run program.
        struct clone_args cloneArgs = {
            .flags = CLONE_VM | CLONE_VFORK,
            .stack = &run_stack[0],
            .stack_size = sizeof(run_stack)
        };
        int32_t programPid = sys_clone3_func(&cloneArgs, sizeof(cloneArgs), run, envp);
        if (programPid < 0) return 1;

        // Wait for program.
        struct siginfo info;
        info.si_status = 0; // Make static analysis happy.
        if (sys_waitid(P_PID, programPid, &info, WEXITED | __WALL, NULL) < 0) return 1;

        char *printStr;
        uint64_t printStrLen;
        if (run_execErrno != 0) {
            info.si_status = run_execErrno;
            printStr = FAILED_RUN;
            printStrLen = hc_STR_LEN(FAILED_RUN);
        } else {
            printStr = SUCCESSFUL_RUN;
            printStrLen = hc_STR_LEN(SUCCESSFUL_RUN);
        }
        char print[
            hc_MAX(hc_STR_LEN(SUCCESSFUL_RUN), hc_STR_LEN(FAILED_RUN)) +
            util_INT32_MAX_CHARS +
            hc_STR_LEN("\n")
        ];
        char *pos = hc_ARRAY_END(print);
        hc_PREPEND_STR(pos, "\n");
        pos = util_intToStr(pos, info.si_status);
        hc_PREPEND(pos, printStr, printStrLen);
        if (util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos) < 0) return 1;
        continue;

        invalidInput:
        if (util_writeAll(util_STDERR, hc_STR_COMMA_LEN(INVALID_INPUT)) < 0) return 1;
    }
}

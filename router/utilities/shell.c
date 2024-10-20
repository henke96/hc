#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone3_func.c"

#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"

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
        if (sys_write(2, hc_STR_COMMA_LEN(PROMPT)) != hc_STR_LEN(PROMPT)) return 1;
        int64_t inputSize = util_readAll(0, &buffer[0], sizeof(buffer));
        if (inputSize < 0) return 1;

        if (sys_write(2, hc_STR_COMMA_LEN("\n")) != hc_STR_LEN("\n")) return 1;
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
            .exit_signal = SIGCHLD,
            .stack = &run_stack[0],
            .stack_size = sizeof(run_stack)
        };
        int32_t programPid = sys_clone3_func(&cloneArgs, sizeof(cloneArgs), run, envp);
        if (programPid < 0) return 1;

        // Wait for program.
        int32_t status = 0; // Make static analysis happy.
        if (sys_wait4(programPid, &status, 0, NULL) < 0) return 1;

        char *printStr;
        int64_t printStrLen;
        if (run_execErrno != 0) {
            status = run_execErrno;
            printStr = FAILED_RUN;
            printStrLen = hc_STR_LEN(FAILED_RUN);
        } else {
            int32_t signal = status & 0x7F;
            if (signal != 0) {
                status = -signal;
            } else {
                status = (status & 0xFF00) >> 8;
            }
            printStr = SUCCESSFUL_RUN;
            printStrLen = hc_STR_LEN(SUCCESSFUL_RUN);
        }
        char *statusStr = util_intToStr(&buffer[sizeof(buffer)], status);
        int64_t statusStrLen = (int64_t)(&buffer[sizeof(buffer)] - statusStr);
        struct iovec_const print[] = {
            { printStr, printStrLen },
            { statusStr, statusStrLen },
            { hc_STR_COMMA_LEN("\n") }
        };
        if (sys_writev(2, &print[0], hc_ARRAY_LEN(print)) != printStrLen + statusStrLen + (int64_t)hc_STR_LEN("\n")) return 1;
        continue;

        invalidInput:
        if (sys_write(2, hc_STR_COMMA_LEN(INVALID_INPUT)) != hc_STR_LEN(INVALID_INPUT)) return 1;
    }
}

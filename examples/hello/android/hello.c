#include "hc/hc.h"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/android/liblog.so.h"
#include "hc/linux/helpers/_start.c"

#define HELLO "Hello!\n"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    int64_t written = sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN(HELLO));
    if (written != hc_STR_LEN(HELLO)) return 1;

    int32_t status = __android_log_write(android_LOG_INFO, NULL, "Hello log!\n");
    if (status < 0) return 1;
    return 0;
}

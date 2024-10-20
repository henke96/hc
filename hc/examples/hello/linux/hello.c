#include "hc/hc.h"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"

#define HELLO "Hello!\n"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    int64_t status = sys_write(2, hc_STR_COMMA_LEN(HELLO));
    if (status != hc_STR_LEN(HELLO)) return 1;
    return 0;
}

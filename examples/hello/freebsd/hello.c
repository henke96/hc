#include "hc/hc.h"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
int32_t start(int32_t, char **);
#include "hc/freebsd/_start.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    int32_t status = printf("Hello!\n");
    if (status < 0) return 1;
    return 0;
}

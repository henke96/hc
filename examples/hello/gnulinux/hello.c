#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/elf.h"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/gnulinux/libc.so.6.h"
#include "hc/linux/gnulinux/_start.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    int32_t status = printf("Hello!\n");
    if (status < 0) return 1;
    return 0;
}

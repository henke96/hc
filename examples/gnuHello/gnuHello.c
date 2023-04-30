#include "hc/hc.h"
#include "hc/util.c"
#include "hc/elf.h"
#include "hc/libc/small.c"
#include "hc/linux/linux.h"
#include "hc/linux/util.c"
#include "hc/linux/sys.c"
#include "hc/linux/gnulinux/libc.so.6.h"
int32_t start(int32_t, char **, char **);
#include "hc/linux/gnulinux/_start.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    printf("Hello world!\n");
    return 0;
}

#include "hc/hc.h"
#include "hc/util.c"
#include "hc/elf.h"
#include "hc/libc/small.c"
#include "hc/linux/linux.h"
#include "hc/linux/util.c"
#include "hc/linux/sys.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/gnulinux/dynamic/libc.so.6.h"
#include "hc/linux/gnulinux/dynamic/libdl.so.2.h"
#include "hc/linux/gnulinux/dynamic/main.c"

static int32_t libcMain(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    void *libcHandle = dlopen("libc.so.6", RTLD_NOW);
    if (dlerror() != NULL) return 1;

    int32_t (*printf)(const char *restrict format, ...) = dlsym(libcHandle, "printf");
    if (dlerror() != NULL) return 1;
    printf("Hello world!\n");
    return 0;
}

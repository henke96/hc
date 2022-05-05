#include "../../src/hc.h"
#include "../../src/util.c"
#include "../../src/libc/small.c"
#include "../../src/linux/linux.h"
#include "../../src/linux/util.c"
#include "../../src/linux/sys.c"
#include "../../src/linux/helpers/_start.c"
#include "../../src/linux/gnulinux/dynamic/libc.so.6.h"
#include "../../src/linux/gnulinux/dynamic/libdl.so.2.h"
#include "../../src/linux/gnulinux/dynamic/main.c"

static int32_t libcMain(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    void *libcHandle = dlopen("libc.so.6", RTLD_NOW);
    if (dlerror() != NULL) return 1;

    int32_t (*printf)(const char *restrict format, ...) = dlsym(libcHandle, "printf");
    if (dlerror() != NULL) return 1;
    printf("Hello world!\n");
    return 0;
}

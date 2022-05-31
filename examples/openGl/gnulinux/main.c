#include "../../../src/hc.h"
#include "../../../src/util.c"
#include "../../../src/gl.h"
#include "../../../src/elf.h"
#include "../../../src/libc/musl.c"
#include "../../../src/linux/linux.h"
#include "../../../src/linux/util.c"
#include "../../../src/linux/sys.c"
#include "../../../src/linux/debug.c"
#include "../../../src/linux/helpers/_start.c"
#include "../../../src/linux/gnulinux/x11.h"
#include "../../../src/linux/gnulinux/xauth.c"
#include "../../../src/linux/gnulinux/x11Client.c"
#include "../../../src/linux/gnulinux/dynamic/libc.so.6.h"
#include "../../../src/linux/gnulinux/dynamic/libdl.so.2.h"
#include "../../../src/linux/gnulinux/dynamic/egl.h"
#include "../../../src/linux/gnulinux/dynamic/egl.c"
#include "../../../src/linux/gnulinux/dynamic/main.c"

static int32_t (*printf)(const char *restrict format, ...);

#define game_EXPORT(NAME) static
#include "gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"
#include "window.c"

static int32_t libcMain(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    void *libcHandle = dlopen("libc.so.6", RTLD_NOW);
    if (dlerror() != NULL) return 1;

    printf = dlsym(libcHandle, "printf");
    if (dlerror() != NULL) return 1;

    int32_t status = window_init(envp);
    if (status < 0) return 1;

    status = window_run();
    window_deinit();
    debug_CHECK(dlclose(libcHandle), == 0);
    if (status < 0) {
        printf("Error while running (%d)\n", status);
        return 1;
    }
    return 0;
}

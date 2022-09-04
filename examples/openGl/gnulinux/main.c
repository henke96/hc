#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/gl.h"
#include "hc/elf.h"
#include "hc/libc/musl.c"
#include "hc/linux/linux.h"
#include "hc/linux/util.c"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/gnulinux/x11.h"
#include "hc/linux/gnulinux/xauth.c"
#include "hc/linux/gnulinux/x11Client.c"
#include "hc/linux/gnulinux/dynamic/libc.so.6.h"
#include "hc/linux/gnulinux/dynamic/libdl.so.2.h"
#include "hc/linux/gnulinux/dynamic/egl.h"
#include "hc/linux/gnulinux/dynamic/egl.c"
#include "hc/linux/gnulinux/dynamic/main.c"

static int32_t (*clock_gettime)(int32_t clock, struct timespec *time);

#define game_EXPORT(NAME) static
#include "gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"
#include "input.c"
#include "window.c"

static int32_t libcMain(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    void *libcHandle = dlopen("libc.so.6", RTLD_NOW);
    if (dlerror() != NULL) return 1;

    clock_gettime = dlsym(libcHandle, "clock_gettime");
    if (dlerror() != NULL) return 1;

    int32_t status = window_init(envp);
    if (status < 0) return 1;

    status = window_run();
    window_deinit();
    debug_CHECK(dlclose(libcHandle), RES == 0);
    if (status < 0) {
        debug_printNum("Error while running (", status, ")\n");
        return 1;
    }
    return 0;
}

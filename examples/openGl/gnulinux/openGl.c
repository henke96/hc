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
#include "hc/linux/gnulinux/x11.h"
#include "hc/linux/gnulinux/xauth.c"
#include "hc/linux/gnulinux/x11Client.c"
#include "hc/linux/gnulinux/dynamic/libc.so.6.h"
#include "hc/linux/gnulinux/dynamic/libdl.so.2.h"
#include "hc/linux/gnulinux/dynamic/egl.h"
#include "hc/linux/gnulinux/dynamic/egl.c"
int32_t start(int32_t, char **, char **);
#include "hc/linux/gnulinux/dynamic/_start.c"

#define game_EXPORT(NAME) static
#include "gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"
#include "input.c"
#include "window.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    int32_t status = window_init(envp);
    if (status < 0) return 1;

    status = window_run();
    window_deinit();
    if (status < 0) {
        debug_printNum("Error while running (", status, ")\n");
        return 1;
    }
    return 0;
}

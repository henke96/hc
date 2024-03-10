#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/gl.h"
#include "hc/elf.h"
#include "hc/x11.h"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/xauth.c"
static int32_t openGl_pageSize;
#define x11Client_PAGE_SIZE openGl_pageSize
#include "hc/linux/x11Client.c"
#include "hc/linux/gnu/libc.so.6.h"
#include "hc/linux/gnu/libdl.so.2.h"
#include "hc/egl.h"
#include "hc/linux/egl.c"
#include "hc/linux/gbm.c"
#include "hc/linux/drmKms.c"
#include "hc/linux/gnu/_start.c"

#define game_EXPORT static
#define gl_GET_PROC_ADDR(LOADER_PTR, FUNC) egl_getProcAddress(LOADER_PTR, FUNC)
#include "../shared/gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"
#include "input.c"
#include "window.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    openGl_pageSize = (int32_t)getauxval(AT_PAGESZ);

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

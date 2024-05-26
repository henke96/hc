#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/gl.h"
#include "hc/elf.h"
#include "hc/x11.h"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#include "hc/freebsd/debug.c"
#include "hc/freebsd/util.c"
#include "hc/freebsd/_start.c"
#include "hc/ix/drm.h"

// Can use the `errno` symbol directly from the main thread.
#define ix_ERRNO(RET) errno
#include "hc/ix/xauth.c"
static int32_t openGl_pageSize;
#define x11Client_PAGE_SIZE openGl_pageSize
#include "hc/ix/x11Client.c"
#include "hc/egl.h"
#include "hc/ix/egl.c"
#include "hc/ix/gbm.c"
#include "hc/ix/drm.c"

#define game_EXPORT static
#define gl_GET_PROC_ADDR(LOADER_PTR, FUNC) egl_getProcAddress(LOADER_PTR, FUNC)
#include "../shared/gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"
#include "../ix/window.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    debug_CHECK(elf_aux_info(AT_PAGESZ, &openGl_pageSize, sizeof(openGl_pageSize)), RES == 0);

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

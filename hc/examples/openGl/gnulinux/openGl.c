#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/gl.h"
#include "hc/elf.h"
#include "hc/x11.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/gnu/gnu.h"
#include "hc/linux/gnu/libc.so.6.h"
#include "hc/linux/gnu/libdl.so.2.h"
#define ix_ERRNO(RET) (*__errno_location())
#include "hc/ix/util.c"
#include "hc/debug.c"
#include "hc/linux/gnu/_start.c"
#include "hc/ix/drm.h"
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
#include "../shared/window.h"
#include "../game.c"
#include "../ix/window.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    openGl_pageSize = (int32_t)getauxval(AT_PAGESZ);

    int32_t status = window_start(envp);
    if (status < 0) return 1;
    return 0;
}

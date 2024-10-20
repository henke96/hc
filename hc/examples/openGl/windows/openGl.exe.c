#include "hc/hc.h"
#include "hc/util.c"
#include "hc/gl.h"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/windows/wgl.h"
#include "hc/windows/windows.h"
#include "hc/windows/util.c"
#include "hc/windows/debug.c"
#include "hc/windows/wgl.c"

#define game_EXPORT static
#define gl_GET_PROC_ADDR(LOADER_PTR, FUNC) wgl_getProcAddress(LOADER_PTR, FUNC)
#include "../shared/gl.c"
#include "../shared/window.h"
#include "../game.c"
#include "window.c"

// Use dedicated GPU.
hc_EXPORT uint32_t NvOptimusEnablement = 0x00000001;
hc_EXPORT uint32_t AmdPowerXpressRequestHighPerformance = 0x00000001;

void noreturn _start(void) {
    AttachConsole(ATTACH_PARENT_PROCESS);
    int32_t status = window_init();
    if (status < 0) {
        debug_printNum("Failed to initialise window (", status, ")\n");
        ExitProcess(1);
    }

    window_run();
    window_deinit();
    ExitProcess(0);
}

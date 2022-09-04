#include "hc/hc.h"
#include "hc/util.c"
#include "hc/gl.h"
#include "hc/libc/musl.c"
#include "hc/windows/wgl.h"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"
#include "hc/windows/wgl.c"

#define game_EXPORT(NAME) static
#include "gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"
#include "input.c"
#include "window.c"

// Use dedicated GPU.
hc_DLLEXPORT uint32_t NvOptimusEnablement = 0x00000001;
hc_DLLEXPORT uint32_t AmdPowerXpressRequestHighPerformance = 0x00000001;

int32_t _fltused; // TODO: Figure out why this is needed.

void noreturn main(void) {
    int32_t status = window_init();
    if (status < 0) {
        debug_printNum("Failed to initialise window (", status, ")\n");
        ExitProcess(1);
    }

    window_run();
    window_deinit();
    ExitProcess(0);
}
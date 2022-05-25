#include "../../../src/hc.h"
#include "../../../src/util.c"
#include "../../../src/gl.h"
#include "../../../src/libc/musl.c"
#include "../../../src/windows/wgl.h"
#include "../../../src/windows/windows.h"
#include "../../../src/windows/debug.c"
#include "../../../src/windows/wgl.c"

#define game_EXPORT(NAME) static
#include "gl.c"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../game.c"
#include "window.c"

// Use dedicated GPU.
hc_DLLEXPORT uint32_t NvOptimusEnablement = 0x00000001;
hc_DLLEXPORT uint32_t AmdPowerXpressRequestHighPerformance = 0x00000001;

int32_t _fltused; // TODO: Figure out why this is needed.

void noreturn main(void) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdOutHandle == INVALID_HANDLE_VALUE) ExitProcess(1);
    WriteFile(stdOutHandle, "Hello!\n", 7, NULL, NULL);

    int32_t status = window_init();
    if (status < 0) {
        debug_printNum("Failed to initialise window (", status, ")\n");
        ExitProcess(1);
    }

    window_run();
    window_deinit();
    ExitProcess(0);
}
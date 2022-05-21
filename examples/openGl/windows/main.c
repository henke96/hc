#include "../../../src/hc.h"
#include "../../../src/util.c"
#include "../../../src/gl.h"
#include "../../../src/libc/musl.c"
#include "../../../src/windows/wgl.h"
#include "../../../src/windows/windows.h"
#include "../../../src/windows/debug.c"
#include "../../../src/windows/wgl.c"

extern void *__ImageBase;

#include "gl.c"
#define game_EXPORT(NAME) static
#include "../game.c"
#include "window.c"

int32_t _fltused; // TODO: Figure out why this is needed.

void noreturn main(void) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdOutHandle == INVALID_HANDLE_VALUE) ExitProcess(1);
    WriteFile(stdOutHandle, "Hello!\n", 7, NULL, NULL);

    int32_t status = window_init();
    if (status < 0) ExitProcess(1);

    status = window_run();
    window_deinit();
    ExitProcess((uint32_t)status);
}
#include "hc/hc.h"
#include "hc/util.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"

void noreturn main(void) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    debug_CHECK(stdOutHandle, RES != INVALID_HANDLE_VALUE);
    WriteFile(stdOutHandle, "Hello!\n", 7, NULL, NULL);
    MessageBoxW(NULL, u"Hey!", u"Yo", MB_OK);
    ExitProcess(0);
}
#include "hc/hc.h"
#include "hc/util.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"

void noreturn _start(void) {
    AttachConsole(ATTACH_PARENT_PROCESS);
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    debug_ASSERT(stdOutHandle != INVALID_HANDLE_VALUE);
    WriteFile(stdOutHandle, hc_STR_COMMA_LEN("Hello!\n"), NULL, NULL);
    MessageBoxW(NULL, u"Hey!", u"Yo", MB_OK);
    ExitProcess(0);
}

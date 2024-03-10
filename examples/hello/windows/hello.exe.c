#include "hc/hc.h"
#include "hc/util.c"
#include "hc/windows/windows.h"

void noreturn _start(void) {
    AttachConsole(ATTACH_PARENT_PROCESS);

    void *stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdoutHandle == INVALID_HANDLE_VALUE) ExitProcess(1);

    uint32_t written;
    int32_t status = WriteFile(stdoutHandle, hc_STR_COMMA_LEN("Hello!\n"), &written, NULL);
    if (status != 1) ExitProcess(1);
    ExitProcess(0);
}

#include "../../../src/hc.h"
#include "../../../src/util.c"
#include "../../../src/windows/windows.h"

extern void *__ImageBase;

static int64_t windowProc(
    void *handle,
    uint32_t message,
    uint64_t wParam,
    int64_t lParam
) {
    switch (message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(handle, message, wParam, lParam);
}

void noreturn main(void) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdOutHandle == INVALID_HANDLE_VALUE) ExitProcess(1);
    WriteFile(stdOutHandle, "Hello!\n", 7, NULL, NULL);

    struct WNDCLASSW windowClass = {
        .instanceHandle = __ImageBase,
        .className = u"TheClass",
        .windowProc = windowProc
    };
    if (!RegisterClassW(&windowClass)) ExitProcess(2);
    void *windowHandle = CreateWindowExW(
        0,
        windowClass.className,
        u"",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        NULL,
        NULL,
        windowClass.instanceHandle,
        NULL
    );
    if (windowHandle == NULL) ExitProcess(3);
    ShowWindow(windowHandle, SW_NORMAL);

    struct MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    ExitProcess(0);
}
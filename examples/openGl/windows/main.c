#include "../../../src/hc.h"
#include "../../../src/util.c"
#include "../../../src/gl.h"
#include "../../../src/windows/wgl.h"
#include "../../../src/windows/windows.h"
#include "../../../src/windows/wgl.c"

#include "gl.c"
#define game_EXPORT(NAME) static
#include "../game.c"

int32_t _fltused; // TODO: Figure out why this is needed.
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
        case WM_PAINT: {
            if (game_draw() < 0 || !SwapBuffers(GetDC(handle))) PostQuitMessage(1);
            return 0;
        }
        case WM_SIZE: {
            int32_t width = lParam & 0xffff;
            int32_t height = (lParam >> 16) & 0xffff;
            game_resize(width, height);
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
        .windowProc = windowProc,
        .style = CS_OWNDC
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

    void *dc = GetDC(windowHandle);
    if (dc == NULL) ExitProcess(4);

    struct PIXELFORMATDESCRIPTOR pfd = {
        .size = sizeof(pfd),
        .version = 1,
        .flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .pixelType = PFD_TYPE_RGBA,
        .colorBits = 24, // TODO: 24 or 32?
        .depthBits = 24,
        .stencilBits = 8,
        .layerType = PFD_MAIN_PLANE
    };
    int32_t format = ChoosePixelFormat(dc, &pfd);
    if (format == 0) ExitProcess(5);

    if (!SetPixelFormat(dc, format, &pfd)) ExitProcess(6);

    struct wgl wgl;
    if (wgl_init(&wgl, dc) < 0) ExitProcess(7);

    // OpenGL 4.3 is compatible with OpenGL ES 3.0.
    int32_t contextAttributes[] = {
        wgl_CONTEXT_MAJOR_VERSION_ARB, 4,
        wgl_CONTEXT_MINOR_VERSION_ARB, 3,
        wgl_CONTEXT_PROFILE_MASK_ARB, wgl_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    if (wgl_updateContext(&wgl, &contextAttributes[0]) < 0) ExitProcess(8);
    wgl_swapInterval(&wgl, 0);

    if (gl_init(&wgl) < 0) ExitProcess(9);
    if (game_init() < 0) ExitProcess(10);

    ShowWindow(windowHandle, SW_NORMAL);

    struct MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    game_deinit();
    wgl_deinit(&wgl);
    ExitProcess(0);
}
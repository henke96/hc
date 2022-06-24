static_assert(!hc_ILP32, "Pointers not 64 bit");

// winnt.h
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3
#define DLL_PROCESS_DETACH 0
#define DLL_PROCESS_VERIFIER 4

// winbase.h
#define INVALID_HANDLE_VALUE ((void *)-1)
#define INVALID_FILE_SIZE ((uint32_t)0xffffffff)
#define INVALID_SET_FILE_POINTER ((uint32_t)-1)
#define INVALID_FILE_ATTRIBUTES ((uint32_t)-1)

#define STD_INPUT_HANDLE ((uint32_t)-10)
#define STD_OUTPUT_HANDLE ((uint32_t)-11)
#define STD_ERROR_HANDLE ((uint32_t)-12)

// wingdi.h
#define PFD_TYPE_RGBA 0
#define PFD_TYPE_COLORINDEX 1

#define PFD_MAIN_PLANE 0
#define PFD_OVERLAY_PLANE 1
#define PFD_UNDERLAY_PLANE (-1)

#define PFD_DOUBLEBUFFER 0x00000001
#define PFD_STEREO 0x00000002
#define PFD_DRAW_TO_WINDOW 0x00000004
#define PFD_DRAW_TO_BITMAP 0x00000008
#define PFD_SUPPORT_GDI 0x00000010
#define PFD_SUPPORT_OPENGL 0x00000020
#define PFD_GENERIC_FORMAT 0x00000040
#define PFD_NEED_PALETTE 0x00000080
#define PFD_NEED_SYSTEM_PALETTE 0x00000100
#define PFD_SWAP_EXCHANGE 0x00000200
#define PFD_SWAP_COPY 0x00000400
#define PFD_SWAP_LAYER_BUFFERS 0x00000800
#define PFD_GENERIC_ACCELERATED 0x00001000
#define PFD_SUPPORT_DIRECTDRAW 0x00002000
#define PFD_DIRECT3D_ACCELERATED 0x00004000
#define PFD_SUPPORT_COMPOSITION 0x00008000

#define PFD_DEPTH_DONTCARE 0x20000000
#define PFD_DOUBLEBUFFER_DONTCARE 0x40000000
#define PFD_STEREO_DONTCARE 0x80000000

struct PIXELFORMATDESCRIPTOR {
    uint16_t size;
    uint16_t version;
    uint32_t flags;
    uint8_t pixelType;
    uint8_t colorBits;
    uint8_t redBits;
    uint8_t redShift;
    uint8_t greenBits;
    uint8_t greenShift;
    uint8_t blueBits;
    uint8_t blueShift;
    uint8_t alphaBits;
    uint8_t alphaShift;
    uint8_t accumBits;
    uint8_t accumRedBits;
    uint8_t accumGreenBits;
    uint8_t accumBlueBits;
    uint8_t accumAlphaBits;
    uint8_t depthBits;
    uint8_t stencilBits;
    uint8_t auxBuffers;
    uint8_t layerType;
    uint8_t reserved;
    uint32_t layerMask;
    uint32_t visibleMask;
    uint32_t damageMask;
};

// winuser.h
#define MB_OK 0x00000000
#define MB_OKCANCEL 0x00000001
#define MB_ABORTRETRYIGNORE 0x00000002
#define MB_YESNOCANCEL 0x00000003
#define MB_YESNO 0x00000004
#define MB_RETRYCANCEL 0x00000005
#define MB_CANCELTRYCONTINUE 0x00000006
#define MB_ICONHAND 0x00000010
#define MB_ICONQUESTION 0x00000020
#define MB_ICONEXCLAMATION 0x00000030
#define MB_ICONASTERISK 0x00000040
#define MB_USERICON 0x00000080
#define MB_ICONWARNING MB_ICONEXCLAMATION
#define MB_ICONERROR MB_ICONHAND
#define MB_ICONINFORMATION MB_ICONASTERISK
#define MB_ICONSTOP MB_ICONHAND
#define MB_DEFBUTTON1 0x00000000
#define MB_DEFBUTTON2 0x00000100
#define MB_DEFBUTTON3 0x00000200
#define MB_DEFBUTTON4 0x00000300
#define MB_APPLMODAL 0x00000000
#define MB_SYSTEMMODAL 0x00001000
#define MB_TASKMODAL 0x00002000
#define MB_HELP 0x00004000
#define MB_NOFOCUS 0x00008000
#define MB_SETFOREGROUND 0x00010000
#define MB_DEFAULT_DESKTOP_ONLY 0x00020000
#define MB_TOPMOST 0x00040000
#define MB_RIGHT 0x00080000
#define MB_RTLREADING 0x00100000
#define MB_SERVICE_NOTIFICATION 0x00200000
#define MB_TYPEMASK 0x0000000F
#define MB_ICONMASK 0x000000F0
#define MB_DEFMASK 0x00000F00
#define MB_MODEMASK 0x00003000
#define MB_MISCMASK 0x0000C000

#define WS_OVERLAPPED 0x00000000
#define WS_POPUP 0x80000000
#define WS_CHILD 0x40000000
#define WS_MINIMIZE 0x20000000
#define WS_VISIBLE 0x10000000
#define WS_DISABLED 0x08000000
#define WS_CLIPSIBLINGS 0x04000000
#define WS_CLIPCHILDREN 0x0200000
#define WS_MAXIMIZE 0x0100000
#define WS_CAPTION 0x00C00000
#define WS_BORDER 0x00800000
#define WS_DLGFRAME 0x00400000
#define WS_VSCROLL 0x00200000
#define WS_HSCROLL 0x00100000
#define WS_SYSMENU 0x00080000
#define WS_THICKFRAME 0x00040000
#define WS_GROUP 0x00020000
#define WS_TABSTOP 0x00010000
#define WS_MINIMIZEBOX 0x00020000
#define WS_MAXIMIZEBOX 0x00010000
#define WS_TILED WS_OVERLAPPED
#define WS_ICONIC WS_MINIMIZE
#define WS_SIZEBOX WS_THICKFRAME
#define WS_TILEDWINDOW WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW (WS_CHILD)

#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002
#define CS_DBLCLKS 0x0008
#define CS_OWNDC 0x0020
#define CS_CLASSDC 0x0040
#define CS_PARENTDC 0x0080
#define CS_NOCLOSE 0x0200
#define CS_SAVEBITS 0x0800
#define CS_BYTEALIGNCLIENT 0x1000
#define CS_BYTEALIGNWINDOW 0x2000
#define CS_GLOBALCLASS 0x4000
#define CS_IME 0x00010000
#define CS_DROPSHADOW 0x00020000

#define CW_USEDEFAULT ((int32_t)0x80000000)

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_NORMAL 1
#define SW_SHOWMINIMIZED 2
#define SW_SHOWMAXIMIZED 3
#define SW_MAXIMIZE 3
#define SW_SHOWNOACTIVATE 4
#define SW_SHOW 5
#define SW_MINIMIZE 6
#define SW_SHOWMINNOACTIVE 7
#define SW_SHOWNA 8
#define SW_RESTORE 9
#define SW_SHOWDEFAULT 10
#define SW_FORCEMINIMIZE 11

#define WM_NULL 0x0000
#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_ACTIVATE 0x0006
#define WA_INACTIVE 0
#define WA_ACTIVE 1
#define WA_CLICKACTIVE 2
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_ENABLE 0x000A
#define WM_SETREDRAW 0x000B
#define WM_SETTEXT 0x000C
#define WM_GETTEXT 0x000D
#define WM_GETTEXTLENGTH 0x000E
#define WM_PAINT 0x000F
#define WM_CLOSE 0x0010
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_SYSCOLORCHANGE 0x0015
#define WM_SHOWWINDOW 0x0018
#define WM_WININICHANGE 0x001A
#define WM_SETTINGCHANGE WM_WININICHANGE
#define WM_DEVMODECHANGE 0x001B
#define WM_ACTIVATEAPP 0x001C
#define WM_FONTCHANGE 0x001D
#define WM_TIMECHANGE 0x001E
#define WM_CANCELMODE 0x001F
#define WM_SETCURSOR 0x0020
#define WM_MOUSEACTIVATE 0x0021
#define WM_CHILDACTIVATE 0x0022
#define WM_QUEUESYNC 0x0023
#define WM_GETMINMAXINFO 0x0024

#define QS_KEY 0x0001
#define QS_MOUSEMOVE 0x0002
#define QS_MOUSEBUTTON 0x0004
#define QS_POSTMESSAGE 0x0008
#define QS_TIMER 0x0010
#define QS_PAINT 0x0020
#define QS_SENDMESSAGE 0x0040
#define QS_HOTKEY 0x0080
#define QS_ALLPOSTMESSAGE 0x0100
#define QS_RAWINPUT 0x0400
#define QS_TOUCH 0x0800
#define QS_POINTER 0x1000

#define QS_MOUSE (QS_MOUSEMOVE | QS_MOUSEBUTTON)
#define QS_INPUT (QS_MOUSE | QS_KEY | QS_RAWINPUT | QS_TOUCH | QS_POINTER)
#define QS_ALLEVENTS (QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY)
#define QS_ALLINPUT (QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY | QS_SENDMESSAGE)

#define PM_NOREMOVE 0x0000
#define PM_REMOVE 0x0001
#define PM_NOYIELD 0x0002

#define PM_QS_INPUT (QS_INPUT << 16)
#define PM_QS_POSTMESSAGE ((QS_POSTMESSAGE | QS_HOTKEY | QS_TIMER) << 16)
#define PM_QS_PAINT (QS_PAINT << 16)
#define PM_QS_SENDMESSAGE (QS_SENDMESSAGE << 16)

struct WNDCLASSW {
    uint32_t style;
    int64_t (*windowProc)(void *handle, uint32_t message, uint64_t wParam, int64_t lParam);
    int32_t clsExtra;
    int32_t wndExtra;
    void *instanceHandle;
    void *iconHandle;
    void *cursorHandle;
    void *backgroundBrushHandle;
    uint16_t *menuName;
    uint16_t *className;
};

struct CREATESTRUCTW {
    void *createParams;
    void *instanceHandle;
    void *menuHandle;
    void *parentWindowHandle;
    int32_t height;
    int32_t width;
    int32_t y;
    int32_t x;
    int32_t style;
    uint16_t *name;
    uint16_t *className;
    uint32_t exStyle;
};

struct POINT {
    int32_t x;
    int32_t y;
};

struct MSG {
    void *windowHandle;
    uint32_t message;
    uint64_t wParam;
    int64_t lParam;
    uint32_t time;
    struct POINT point;
};

// kernel32.lib
hc_DLLIMPORT uint32_t GetLastError(void);

hc_DLLIMPORT int32_t AllocConsole(void);
hc_DLLIMPORT int32_t FreeConsole(void);
hc_DLLIMPORT int32_t AttachConsole(uint32_t processId);

hc_DLLIMPORT noreturn void ExitProcess(uint32_t exitCode);
hc_DLLIMPORT int32_t TerminateProcess(void *handle, uint32_t exitCode);
hc_DLLIMPORT void *GetCurrentProcess(void);

hc_DLLIMPORT void *GetStdHandle(uint32_t type);
hc_DLLIMPORT int32_t WriteFile(void *fileHandle, const void *buffer, uint32_t numberOfBytesToWrite, uint32_t *numberOfBytesWritten, void *overlapped);

hc_DLLIMPORT void *LoadLibraryW(const uint16_t *libFileName);
hc_DLLIMPORT int32_t FreeLibrary(void *dlHandle);
hc_DLLIMPORT void *GetProcAddress(void *moduleHandle, const char *procName);

hc_DLLIMPORT int32_t QueryPerformanceFrequency(int64_t *frequency);
hc_DLLIMPORT int32_t QueryPerformanceCounter(int64_t *count);

// user32.lib
hc_DLLIMPORT int32_t MessageBoxW(void *windowHandle, const uint16_t *text, const uint16_t *caption, uint32_t type);
hc_DLLIMPORT uint16_t RegisterClassW(const struct WNDCLASSW *windowClass);
hc_DLLIMPORT int32_t UnregisterClassW(const uint16_t *className, void *instanceHandle);
hc_DLLIMPORT int64_t DefWindowProcW(void *windowHandle, uint32_t message, uint64_t wParam, int64_t lParam);
hc_DLLIMPORT void *CreateWindowExW(
    uint32_t exStyle,
    uint16_t *className,
    uint16_t *windowName,
    uint32_t style,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height,
    void *parentHandle,
    void *menuHandle,
    void *instanceHandle,
    void *param
);
hc_DLLIMPORT int32_t ShowWindow(void *windowHandle, int32_t showCommand);
hc_DLLIMPORT int32_t DestroyWindow(void *windowHandle);

hc_DLLIMPORT int32_t GetMessageW(struct MSG *msg, void *windowHandle, uint32_t msgFilterMin, uint32_t msgFilterMax);
hc_DLLIMPORT int32_t PeekMessageW(struct MSG *msg, void *windowHandle, uint32_t msgFilterMin, uint32_t msgFilterMax, uint32_t removeMsg);
hc_DLLIMPORT int32_t TranslateMessage(const struct MSG *msg);
hc_DLLIMPORT int64_t DispatchMessageW(const struct MSG *msg);
hc_DLLIMPORT void PostQuitMessage(int32_t exitCode);
hc_DLLIMPORT int32_t PostMessageW(void *windowHandle, uint32_t msg, uint64_t wParam, int64_t lParam);
hc_DLLIMPORT void *GetDC(void *windowHandle);
hc_DLLIMPORT int32_t ReleaseDC(void *windowHandle, void *dc);

// gdi32.lib
hc_DLLIMPORT int32_t ChoosePixelFormat(void *dc, const struct PIXELFORMATDESCRIPTOR *pfd);
hc_DLLIMPORT int32_t SetPixelFormat(void *dc, int32_t format, const struct PIXELFORMATDESCRIPTOR *pfd);
hc_DLLIMPORT int32_t DescribePixelFormat(void *dc, int32_t pixelFormat, uint32_t size, struct PIXELFORMATDESCRIPTOR *pfd);
hc_DLLIMPORT int32_t SwapBuffers(void *dc);
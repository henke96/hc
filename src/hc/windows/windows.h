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
#define IMAGE_BITMAP 0
#define IMAGE_ICON 1
#define IMAGE_CURSOR 2
#define IMAGE_ENHMETAFILE 3

#define IDC_ARROW ((void *)32512)
#define IDC_IBEAM ((void *)32513)
#define IDC_WAIT ((void *)32514)
#define IDC_CROSS ((void *)32515)
#define IDC_UPARROW ((void *)32516)
#define IDC_SIZE ((void *)32640)
#define IDC_ICON ((void *)32641)
#define IDC_SIZENWSE ((void *)32642)
#define IDC_SIZENESW ((void *)32643)
#define IDC_SIZEWE ((void *)32644)
#define IDC_SIZENS ((void *)32645)
#define IDC_SIZEALL ((void *)32646)
#define IDC_NO ((void *)32648)
#define IDC_HAND ((void *)32649)
#define IDC_APPSTARTING ((void *)32650)
#define IDC_HELP ((void *)32651)
#define IDC_PIN ((void *)32671)
#define IDC_PERSON ((void *)32672)

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

#define WM_PAINTICON 0x0026
#define WM_ICONERASEBKGND 0x0027
#define WM_NEXTDLGCTL 0x0028
#define WM_SPOOLERSTATUS 0x002A
#define WM_DRAWITEM 0x002B
#define WM_MEASUREITEM 0x002C
#define WM_DELETEITEM 0x002D
#define WM_VKEYTOITEM 0x002E
#define WM_CHARTOITEM 0x002F
#define WM_SETFONT 0x0030
#define WM_GETFONT 0x0031
#define WM_SETHOTKEY 0x0032
#define WM_GETHOTKEY 0x0033
#define WM_QUERYDRAGICON 0x0037
#define WM_COMPAREITEM 0x0039
#define WM_GETOBJECT 0x003D
#define WM_COMPACTING 0x0041
#define WM_COMMNOTIFY 0x0044
#define WM_WINDOWPOSCHANGING 0x0046
#define WM_WINDOWPOSCHANGED 0x0047
#define WM_POWER 0x0048

#define PWR_OK 1
#define PWR_FAIL (-1)
#define PWR_SUSPENDREQUEST 1
#define PWR_SUSPENDRESUME 2
#define PWR_CRITICALRESUME 3

#define WM_COPYDATA 0x004A
#define WM_CANCELJOURNAL 0x004B

#define WM_NOTIFY 0x004E
#define WM_INPUTLANGCHANGEREQUEST 0x0050
#define WM_INPUTLANGCHANGE 0x0051
#define WM_TCARD 0x0052
#define WM_HELP 0x0053
#define WM_USERCHANGED 0x0054
#define WM_NOTIFYFORMAT 0x0055

#define NFR_ANSI 1
#define NFR_UNICODE 2
#define NF_QUERY 3
#define NF_REQUERY 4

#define WM_CONTEXTMENU 0x007B
#define WM_STYLECHANGING 0x007C
#define WM_STYLECHANGED 0x007D
#define WM_DISPLAYCHANGE 0x007E
#define WM_GETICON 0x007F
#define WM_SETICON 0x0080

#define WM_NCCREATE 0x0081
#define WM_NCDESTROY 0x0082
#define WM_NCCALCSIZE 0x0083
#define WM_NCHITTEST 0x0084
#define WM_NCPAINT 0x0085
#define WM_NCACTIVATE 0x0086
#define WM_GETDLGCODE 0x0087
#define WM_SYNCPAINT 0x0088
#define WM_NCMOUSEMOVE 0x00A0
#define WM_NCLBUTTONDOWN 0x00A1
#define WM_NCLBUTTONUP 0x00A2
#define WM_NCLBUTTONDBLCLK 0x00A3
#define WM_NCRBUTTONDOWN 0x00A4
#define WM_NCRBUTTONUP 0x00A5
#define WM_NCRBUTTONDBLCLK 0x00A6
#define WM_NCMBUTTONDOWN 0x00A7
#define WM_NCMBUTTONUP 0x00A8
#define WM_NCMBUTTONDBLCLK 0x00A9

#define WM_NCXBUTTONDOWN 0x00AB
#define WM_NCXBUTTONUP 0x00AC
#define WM_NCXBUTTONDBLCLK 0x00AD
#define WM_INPUT_DEVICE_CHANGE 0x00fe
#define WM_INPUT 0x00FF
#define WM_KEYFIRST 0x0100
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102
#define WM_DEADCHAR 0x0103
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_SYSCHAR 0x0106
#define WM_SYSDEADCHAR 0x0107
#define WM_UNICHAR 0x0109
#define WM_KEYLAST 0x0109
#define UNICODE_NOCHAR 0xFFFF
#define WM_IME_STARTCOMPOSITION 0x010D
#define WM_IME_ENDCOMPOSITION 0x010E
#define WM_IME_COMPOSITION 0x010F
#define WM_IME_KEYLAST 0x010F
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_HSCROLL 0x0114
#define WM_VSCROLL 0x0115
#define WM_INITMENU 0x0116
#define WM_INITMENUPOPUP 0x0117
#define WM_MENUSELECT 0x011F
#define WM_GESTURE 0x0119
#define WM_GESTURENOTIFY 0x011A
#define WM_MENUCHAR 0x0120
#define WM_ENTERIDLE 0x0121

#define WM_MENURBUTTONUP 0x0122
#define WM_MENUDRAG 0x0123
#define WM_MENUGETOBJECT 0x0124
#define WM_UNINITMENUPOPUP 0x0125
#define WM_MENUCOMMAND 0x0126
#define WM_CHANGEUISTATE 0x0127
#define WM_UPDATEUISTATE 0x0128
#define WM_QUERYUISTATE 0x0129

#define UIS_SET 1
#define UIS_CLEAR 2
#define UIS_INITIALIZE 3

#define UISF_HIDEFOCUS 0x1
#define UISF_HIDEACCEL 0x2
#define UISF_ACTIVE 0x4

#define WM_CTLCOLORMSGBOX 0x0132
#define WM_CTLCOLOREDIT 0x0133
#define WM_CTLCOLORLISTBOX 0x0134
#define WM_CTLCOLORBTN 0x0135
#define WM_CTLCOLORDLG 0x0136
#define WM_CTLCOLORSCROLLBAR 0x0137
#define WM_CTLCOLORSTATIC 0x0138
#define MN_GETHMENU 0x01E1

#define WM_MOUSEFIRST 0x0200
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_LBUTTONDBLCLK 0x0203
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_RBUTTONDBLCLK 0x0206
#define WM_MBUTTONDOWN 0x0207
#define WM_MBUTTONUP 0x0208
#define WM_MBUTTONDBLCLK 0x0209
#define WM_MOUSEWHEEL 0x020A
#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP 0x020C
#define WM_XBUTTONDBLCLK 0x020D
#define WM_MOUSEHWHEEL 0x020e
#define WM_MOUSELAST 0x020e

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
    int32_t __pad;
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
    int32_t __pad;
    uint16_t *name;
    uint16_t *className;
    uint32_t exStyle;
    int32_t __pad2;
};

struct POINT {
    int32_t x;
    int32_t y;
};

struct MSG {
    void *windowHandle;
    uint32_t message;
    int32_t __pad;
    uint64_t wParam;
    int64_t lParam;
    uint32_t time;
    struct POINT point;
    int32_t __pad2;
};

struct RECT {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

#define GWL_STYLE (-16)
#define GWL_EXSTYLE (-20)
#define GWL_ID (-12)

// Raw input.
#define RIM_INPUT 0
#define RIM_INPUTSINK 1

struct RAWINPUTHEADER {
    uint32_t type;
    uint32_t size;
    void *deviceHandle;
    uint64_t wParam;
};

#define RIM_TYPEMOUSE 0
#define RIM_TYPEKEYBOARD 1
#define RIM_TYPEHID 2
#define RIM_TYPEMAX 2

struct RAWMOUSE {
    uint16_t flags;
    int16_t __pad;
    union {
        uint32_t buttons;
        struct {
            uint16_t buttonFlags;
            uint16_t buttonData;
        };
    };
    uint32_t rawButtons;
    int32_t lastX;
    int32_t lastY;
    uint32_t extraInformation;
};

#define RI_MOUSE_LEFT_BUTTON_DOWN 0x0001
#define RI_MOUSE_LEFT_BUTTON_UP 0x0002
#define RI_MOUSE_RIGHT_BUTTON_DOWN 0x0004
#define RI_MOUSE_RIGHT_BUTTON_UP 0x0008
#define RI_MOUSE_MIDDLE_BUTTON_DOWN 0x0010
#define RI_MOUSE_MIDDLE_BUTTON_UP 0x0020
#define RI_MOUSE_BUTTON_4_DOWN 0x0040
#define RI_MOUSE_BUTTON_4_UP 0x0080
#define RI_MOUSE_BUTTON_5_DOWN 0x0100
#define RI_MOUSE_BUTTON_5_UP 0x0200
#define RI_MOUSE_WHEEL 0x0400
#define RI_MOUSE_HWHEEL 0x0800

#define RI_MOUSE_BUTTON_1_DOWN RI_MOUSE_LEFT_BUTTON_DOWN
#define RI_MOUSE_BUTTON_1_UP RI_MOUSE_LEFT_BUTTON_UP
#define RI_MOUSE_BUTTON_2_DOWN RI_MOUSE_RIGHT_BUTTON_DOWN
#define RI_MOUSE_BUTTON_2_UP RI_MOUSE_RIGHT_BUTTON_UP
#define RI_MOUSE_BUTTON_3_DOWN RI_MOUSE_MIDDLE_BUTTON_DOWN
#define RI_MOUSE_BUTTON_3_UP RI_MOUSE_MIDDLE_BUTTON_UP

#define MOUSE_MOVE_RELATIVE 0
#define MOUSE_MOVE_ABSOLUTE 1
#define MOUSE_VIRTUAL_DESKTOP 0x02
#define MOUSE_ATTRIBUTES_CHANGED 0x04
#define MOUSE_MOVE_NOCOALESCE 0x08

struct RAWKEYBOARD {
    uint16_t makeCode;
    uint16_t flags;
    uint16_t reserved;
    uint16_t vKey;
    uint32_t message;
    uint32_t extraInformation;
};

#define KEYBOARD_OVERRUN_MAKE_CODE 0xFF

#define RI_KEY_MAKE 0
#define RI_KEY_BREAK 1
#define RI_KEY_E0 2
#define RI_KEY_E1 4

struct RAWHID {
    uint32_t sizeHid;
    uint32_t count;
    uint8_t rawData[];
};

struct RAWINPUT {
    struct RAWINPUTHEADER header;
    union {
        struct RAWMOUSE mouse;
        struct RAWKEYBOARD keyboard;
        struct RAWHID hid;
    } data;
};

#define RID_INPUT 0x10000003
#define RID_HEADER 0x10000005

struct RAWINPUTDEVICE {
    uint16_t usagePage;
    uint16_t usage;
    uint32_t flags;
    void *targetWindowHandle;
};

#define RIDEV_REMOVE 0x00000001
#define RIDEV_EXCLUDE 0x00000010
#define RIDEV_PAGEONLY 0x00000020
#define RIDEV_NOLEGACY 0x00000030
#define RIDEV_INPUTSINK 0x00000100
#define RIDEV_CAPTUREMOUSE 0x00000200
#define RIDEV_NOHOTKEYS 0x00000200
#define RIDEV_APPKEYS 0x00000400
#define RIDEV_EXINPUTSINK 0x00001000
#define RIDEV_DEVNOTIFY 0x00002000

struct MONITORINFOEXW {
    struct {
      uint32_t size;
      struct RECT monitorRect;
      struct RECT workAreaRect;
      uint32_t flags;
    };
    uint16_t device[32];
};

#define MONITOR_DEFAULTTONULL 0x00000000
#define MONITOR_DEFAULTTOPRIMARY 0x00000001
#define MONITOR_DEFAULTTONEAREST 0x00000002

#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004
#define SWP_NOREDRAW 0x0008
#define SWP_NOACTIVATE 0x0010
#define SWP_FRAMECHANGED 0x0020
#define SWP_SHOWWINDOW 0x0040
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOCOPYBITS 0x0100
#define SWP_NOOWNERZORDER 0x0200
#define SWP_NOSENDCHANGING 0x0400

#define SWP_DEFERERASE 0x2000
#define SWP_ASYNCWINDOWPOS 0x4000

#define HWND_TOP ((void *)0)
#define HWND_BOTTOM ((void *)1)
#define HWND_TOPMOST ((void *)-1)
#define HWND_NOTOPMOST ((void *)-2)

struct WINDOWPLACEMENT {
    uint32_t length;
    uint32_t flags;
    uint32_t showCmd;
    struct POINT ptMinPosition;
    struct POINT ptMaxPosition;
    struct RECT normalPositionRect;
    struct RECT deviceRect;
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
hc_DLLIMPORT int32_t GetClientRect(void *windowHandle, struct RECT *rect);
hc_DLLIMPORT int32_t ClientToScreen(void *windowHandle, struct POINT *point);
hc_DLLIMPORT int32_t GetWindowRect(void *windowHandle, struct RECT *rect);
hc_DLLIMPORT int32_t GetWindowLongW(void *windowHandle, int32_t index);
hc_DLLIMPORT int32_t SetWindowLongW(void *windowHandle, int32_t index, int32_t newLong);
hc_DLLIMPORT int32_t SetWindowPos(void *windowHandle, void *windowHandleInsertAfter, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t flags);
hc_DLLIMPORT int32_t GetWindowPlacement(void *windowHandle, struct WINDOWPLACEMENT *windowPlacement);
hc_DLLIMPORT int32_t SetWindowPlacement(void *windowHandle, const struct WINDOWPLACEMENT *windowPlacement);
hc_DLLIMPORT int32_t ClipCursor(const struct RECT *rect);
hc_DLLIMPORT int32_t ShowCursor(int32_t show);
hc_DLLIMPORT void *LoadCursorW(void *instanceHandle, const uint16_t *name);
hc_DLLIMPORT void *LoadImageW(void *instanceHandle, const uint16_t *name, uint32_t type, int32_t width, int32_t height, uint32_t flags);

hc_DLLIMPORT int32_t GetMessageW(struct MSG *msg, void *windowHandle, uint32_t msgFilterMin, uint32_t msgFilterMax);
hc_DLLIMPORT int32_t PeekMessageW(struct MSG *msg, void *windowHandle, uint32_t msgFilterMin, uint32_t msgFilterMax, uint32_t removeMsg);
hc_DLLIMPORT int32_t TranslateMessage(const struct MSG *msg);
hc_DLLIMPORT int64_t DispatchMessageW(const struct MSG *msg);
hc_DLLIMPORT void PostQuitMessage(int32_t exitCode);
hc_DLLIMPORT int32_t PostMessageW(void *windowHandle, uint32_t msg, uint64_t wParam, int64_t lParam);
hc_DLLIMPORT void *GetDC(void *windowHandle);
hc_DLLIMPORT int32_t ReleaseDC(void *windowHandle, void *dc);

hc_DLLIMPORT int32_t RegisterRawInputDevices(const struct RAWINPUTDEVICE *rawInputDevices, uint32_t numDevices, uint32_t size);
hc_DLLIMPORT uint32_t GetRawInputData(void *rawInputHandle, uint32_t command, void *data, uint32_t *dataSize, uint32_t headerSize);

hc_DLLIMPORT int32_t GetMonitorInfoW(void *monitorHandle, struct MONITORINFOEXW *monitorInfo);
hc_DLLIMPORT void *MonitorFromWindow(void *windowHandle, uint32_t flags);

// gdi32.lib
hc_DLLIMPORT int32_t ChoosePixelFormat(void *dc, const struct PIXELFORMATDESCRIPTOR *pfd);
hc_DLLIMPORT int32_t SetPixelFormat(void *dc, int32_t format, const struct PIXELFORMATDESCRIPTOR *pfd);
hc_DLLIMPORT int32_t DescribePixelFormat(void *dc, int32_t pixelFormat, uint32_t size, struct PIXELFORMATDESCRIPTOR *pfd);
hc_DLLIMPORT int32_t SwapBuffers(void *dc);
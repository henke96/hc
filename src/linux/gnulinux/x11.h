// First byte from server describes message type. Values 2-127 are events.
#define x11_TYPE_ERROR 0
#define x11_TYPE_REPLY 1
#define x11_TYPE_SYNTHETIC_BIT 0x80
#define x11_TYPE_MASK 0x7F

#define x11_COPY_FROM_PARENT 0
#define x11_INPUT_OUTPUT 1
#define x11_INPUT_ONLY 2

// Set-of-event bits.
#define x11_EVENT_KEY_PRESS_BIT 0x1
#define x11_EVENT_KEY_RELEASE_BIT 0x2
#define x11_EVENT_BUTTON_PRESS_BIT 0x4
#define x11_EVENT_BUTTON_RELEASE_BIT 0x8
#define x11_EVENT_ENTER_WINDOW_BIT 0x10
#define x11_EVENT_LEAVE_WINDOW_BIT 0x20
#define x11_EVENT_POINTER_MOTION_BIT 0x40
#define x11_EVENT_POINTER_MOTION_HINT_BIT 0x80
#define x11_EVENT_BUTTON1_MOTION_BIT 0x100
#define x11_EVENT_BUTTON2_MOTION_BIT 0x200
#define x11_EVENT_BUTTON3_MOTION_BIT 0x400
#define x11_EVENT_BUTTON4_MOTION_BIT 0x800
#define x11_EVENT_BUTTON5_MOTION_BIT 0x1000
#define x11_EVENT_BUTTON_MOTION_BIT 0x2000
#define x11_EVENT_KEYMAP_STATE_BIT 0x4000
#define x11_EVENT_EXPOSURE_BIT 0x8000
#define x11_EVENT_VISIBILITY_CHANGE_BIT 0x10000
#define x11_EVENT_STRUCTURE_NOTIFY_BIT 0x20000
#define x11_EVENT_RESIZE_REDIRECT_BIT 0x40000
#define x11_EVENT_SUBSTRUCTURE_NOTIFY_BIT 0x80000
#define x11_EVENT_SUBSTRUCTURE_REDIRECT_BIT 0x100000
#define x11_EVENT_FOCUS_CHANGE_BIT 0x200000
#define x11_EVENT_PROPERTY_CHANGE_BIT 0x400000
#define x11_EVENT_COLORMAP_CHANGE_BIT 0x800000
#define x11_EVENT_OWNER_GRAB_BUTTON_BIT 0x1000000

struct x11_format {
    uint8_t depth;
    uint8_t bitsPerPixel;
    uint8_t scanlinePad;
    uint8_t __pad[5];
};

struct x11_visualType {
    uint32_t visualId;
    uint8_t class;
    uint8_t bitsPerRgbValue;
    uint16_t colormapEntries;
    uint32_t redMask;
    uint32_t greenMask;
    uint32_t blueMask;
    uint8_t __pad[4];
};

struct x11_depth {
    uint8_t depth;
    uint8_t __pad;
    uint16_t numVisualTypes;
    uint8_t __pad2[4];
    struct x11_visualType visualTypes[];
};

struct x11_screen {
    uint32_t windowId;
    uint32_t defaultColormap;
    uint32_t whitePixel;
    uint32_t blackPixel;
    uint32_t currentInputMasks;
    uint16_t width;
    uint16_t height;
    uint16_t widthMillimeter;
    uint16_t heightMillimeter;
    uint16_t minInstalledMaps;
    uint16_t maxInstalledMaps;
    uint32_t rootVisual;
    uint8_t backingStores;
    uint8_t saveUnders;
    uint8_t rootDepth;
    uint8_t numAllowedDepths;
    struct x11_depth allowedDepths[];
};

// Protocol setup.
#define x11_setupRequest_LITTLE_ENDIAN 0x6c
#define x11_setupRequest_PROTOCOL_MAJOR_VERSION 11
#define x11_setupRequest_PROTOCOL_MINOR_VERSION 0
struct x11_setupRequest {
    uint8_t byteOrder;
    uint8_t __pad;
    uint16_t protocolMajorVersion;
    uint16_t protocolMinorVersion;
    uint16_t authProtocolNameLength;
    uint16_t authProtocolDataLength;
    uint8_t __pad2[2];
    uint8_t data[]; // char authProtocolName[authProtocolNameLength];
                    // __pad3[util_PAD_BYTES(authProtocolNameLength, 4)];
                    // uint8_t authProtocolData[authProtocolDataLength];
                    // __pad4[util_PAD_BYTES(authProtocolDataLength, 4)];
};

#define x11_setupResponse_SUCCESS 1
struct x11_setupResponse {
    uint8_t status;
    uint8_t __pad;
    uint16_t protocolMajorVersion;
    uint16_t protocolMinorVersion;
    uint16_t length;
    uint32_t releaseNumber;
    uint32_t resourceIdBase;
    uint32_t resourceIdMask;
    uint32_t motionBufferSize;
    uint16_t vendorLength;
    uint16_t maximumRequestLength;
    uint8_t numRoots;
    uint8_t numPixmapFormats;
    uint8_t imageByteOrder;
    uint8_t bitmapFormatBitOrder;
    uint8_t bitmapFormatScanlineUnit;
    uint8_t bitmapFormatScanlinePad;
    uint8_t minKeycode;
    uint8_t maxKeycode;
    uint8_t __pad2[4];
    uint8_t data[]; // char vendor[vendorLength];
                    // uint8_t __pad[util_PAD_BYTES(vendorLength, 4)];
                    // struct x11_format pixmapFormats[numPixmapFormats];
                    // struct x11_screen roots[numRoots]; (x11_screen is variable length!)
};

// Protocol requests, responses and events.

#define x11_createWindow_OPCODE 1
// Bits for valueMask.
#define x11_createWindow_BACKGROUND_PIXMAP 0x1
#define x11_createWindow_BACKGROUND_PIXEL 0x2
#define x11_createWindow_BORDER_PIXMAP 0x3
#define x11_createWindow_BORDER_PIXEL 0x8
#define x11_createWindow_BIT_GRAVITY 0x10
#define x11_createWindow_WIN_GRAVITY 0x20
#define x11_createWindow_BACKING_STORE 0x40
#define x11_createWindow_BACKING_PLANES 0x80
#define x11_createWindow_BACKING_PIXEL 0x100
#define x11_createWindow_OVERRIDE_REDIRECT 0x200
#define x11_createWindow_SAVE_UNDER 0x400
#define x11_createWindow_EVENT_MASK 0x800
#define x11_createWindow_DO_NOT_PROPAGATE_MASK 0x1000
#define x11_createWindow_COLORMAP 0x2000
#define x11_createWindow_CURSOR 0x4000

struct x11_createWindow {
    uint8_t opcode;
    uint8_t depth;
    uint16_t length;
    uint32_t windowId;
    uint32_t parentId;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t borderWidth;
    uint16_t class;
    uint32_t visualId;
    uint32_t valueMask;
    uint32_t values[];
};

#define x11_getWindowAttributes_OPCODE 3
struct x11_getWindowAttributes {
    uint8_t opcode;
    uint8_t __pad;
    uint16_t length;
    uint32_t windowId;
};

struct x11_getWindowAttributesReponse {
    uint8_t type;
    uint8_t backingStore;
    uint16_t sequenceNumber;
    uint32_t length;
    uint32_t visualId;
    uint16_t class;
    uint8_t bitGravity;
    uint8_t winGravity;
    uint32_t backingPlanes;
    uint32_t backingPixel;
    uint8_t saveUnder;
    uint8_t mapIsInstalled;
    uint8_t mapState;
    uint8_t overrideRedirect;
    uint32_t colormap;
    uint32_t allEventMask;
    uint32_t yourEventMask;
    uint16_t doNotPropagateMask;
    uint8_t __pad[2];
};

#define x11_mapWindow_OPCODE 8
struct x11_mapWindow {
    uint8_t opcode;
    uint8_t __pad;
    uint16_t length;
    uint32_t windowId;
};

#define x11_mapNotify_TYPE 19
struct x11_mapNotify {
    uint8_t type;
    uint8_t __pad;
    uint16_t sequenceNumber;
    uint32_t eventWindowId;
    uint32_t windowId;
    uint8_t overrideRedirect;
    uint8_t __pad2[19];
};

#define x11_configureNotify_TYPE 22
struct x11_configureNotify {
    uint8_t type;
    uint8_t __pad;
    uint16_t sequenceNumber;
    uint32_t eventWindowId;
    uint32_t windowId;
    uint32_t aboveSiblingWindowId;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t borderWidth;
    uint8_t overrideRedirect;
    uint8_t __pad2[5];
};

#define x11_expose_TYPE 12
struct x11_expose {
    uint8_t type;
    uint8_t __pad;
    uint16_t sequenceNumber;
    uint32_t windowId;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t count;
    uint8_t __pad2[14];
};
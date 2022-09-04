// First byte from server describes message type. Values 2-127 are events.
#define x11_TYPE_ERROR 0
#define x11_TYPE_REPLY 1
#define x11_TYPE_SYNTHETIC_BIT 0x80
#define x11_TYPE_MASK 0x7F

#define x11_FALSE 0
#define x11_TRUE 1

#define x11_COPY_FROM_PARENT 0
#define x11_INPUT_OUTPUT 1
#define x11_INPUT_ONLY 2

#define x11_ATOM_NONE 0
#define x11_ATOM_ATOM 4
#define x11_ATOM_CARDINAL 6

#define x11_NET_WM_STATE_REMOVE 0
#define x11_NET_WM_STATE_ADD 1
#define x11_NET_WM_STATE_TOGGLE 2

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
                    // __pad3[math_PAD_BYTES(authProtocolNameLength, 4)];
                    // uint8_t authProtocolData[authProtocolDataLength];
                    // __pad4[math_PAD_BYTES(authProtocolDataLength, 4)];
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
                    // uint8_t __pad[math_PAD_BYTES(vendorLength, 4)];
                    // struct x11_format pixmapFormats[numPixmapFormats];
                    // struct x11_screen roots[numRoots]; (x11_screen is variable length!)
};

// Protocol events.
#define x11_keyPress_TYPE 2
struct x11_keyPress {
    uint8_t type;
    uint8_t detail;
    uint16_t sequenceNumber;
    uint32_t timeMs;
    uint32_t windowId;
    uint32_t eventWindowId;
    uint32_t childWindowId;
    int16_t rootX;
    int16_t rootY;
    int16_t eventX;
    int16_t eventY;
    uint16_t state;
    uint8_t sameScreen;
    uint8_t __pad;
};

#define x11_keyRelease_TYPE 3
struct x11_keyRelease {
    uint8_t type;
    uint8_t detail;
    uint16_t sequenceNumber;
    uint32_t timeMs;
    uint32_t windowId;
    uint32_t eventWindowId;
    uint32_t childWindowId;
    int16_t rootX;
    int16_t rootY;
    int16_t eventX;
    int16_t eventY;
    uint16_t state;
    uint8_t sameScreen;
    uint8_t __pad;
};

#define x11_buttonPress_TYPE 4
struct x11_buttonPress {
    uint8_t type;
    uint8_t detail;
    uint16_t sequenceNumber;
    uint32_t timeMs;
    uint32_t windowId;
    uint32_t eventWindowId;
    uint32_t childWindowId;
    int16_t rootX;
    int16_t rootY;
    int16_t eventX;
    int16_t eventY;
    uint16_t state;
    uint8_t sameScreen;
    uint8_t __pad;
};

#define x11_buttonRelease_TYPE 5
struct x11_buttonRelease {
    uint8_t type;
    uint8_t detail;
    uint16_t sequenceNumber;
    uint32_t timeMs;
    uint32_t windowId;
    uint32_t eventWindowId;
    uint32_t childWindowId;
    int16_t rootX;
    int16_t rootY;
    int16_t eventX;
    int16_t eventY;
    uint16_t state;
    uint8_t sameScreen;
    uint8_t __pad;
};

#define x11_motionNotify_TYPE 6
struct x11_motionNotify {
    uint8_t type;
    uint8_t detail;
    uint16_t sequenceNumber;
    uint32_t timeMs;
    uint32_t windowId;
    uint32_t eventWindowId;
    uint32_t childWindowId;
    int16_t rootX;
    int16_t rootY;
    int16_t eventX;
    int16_t eventY;
    uint16_t keyButtonState;
    uint8_t sameScreen;
    uint8_t __pad;
};

#define x11_focusOut_TYPE 10
struct x11_focusOut {
    uint8_t type;
    uint8_t detail;
    uint16_t sequenceNumber;
    uint32_t eventWindowId;
    uint8_t mode;
    uint8_t __pad[23];
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

#define x11_clientMessage_TYPE 33
struct x11_clientMessage {
    uint8_t type;
    uint8_t format;
    uint16_t sequenceNumber;
    uint32_t window;
    uint32_t atom;
    union {
        uint8_t data8[20];
        uint16_t data16[10];
        uint32_t data32[5];
    };
};

#define x11_genericEvent_TYPE 35
struct x11_genericEvent {
    uint8_t type;
    uint8_t extension;
    uint16_t sequenceNumber;
    uint32_t length;
    uint16_t eventType;
    uint8_t data[];
};

// Protocol requests and responses.
struct x11_genericResponse {
    uint8_t type;
    uint8_t extra;
    uint16_t sequenceNumber;
    uint8_t data[];
};

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

#define x11_internAtom_OPCODE 16
struct x11_internAtom {
    uint8_t opcode;
    uint8_t onlyIfExists;
    uint16_t length;
    uint16_t nameLength;
    uint16_t __pad;
    uint8_t data[]; // char name[nameLength];
                    // uint8_t __pad2[math_PAD_BYTES(nameLength, 4)];
};

struct x11_internAtomResponse {
    uint8_t type;
    uint8_t __pad;
    uint16_t sequenceNumber;
    uint32_t length;
    uint32_t atom;
    uint8_t __pad2[20];
};

#define x11_changeProperty_OPCODE 18
#define x11_changeProperty_REPLACE 0
#define x11_changeProperty_PREPEND 1
#define x11_changeProperty_APPEND 2
struct x11_changeProperty {
    uint8_t opcode;
    uint8_t mode;
    uint16_t length;
    uint32_t window;
    uint32_t property;
    uint32_t type;
    uint8_t format; // 8, 16 or 32 bits.
    uint8_t __pad[3];
    uint32_t dataLength; // In `format` units.
    uint8_t data[]; // format_t data[dataLength];
                    // uint8_t __pad2[math_PAD_BYTES(sizeof(format_t) * dataLength, 4)];
};

#define x11_sendEvent_OPCODE 25
struct x11_sendEvent {
    uint8_t opcode;
    uint8_t propagate;
    uint16_t length;
    uint32_t destWindowId;
    uint32_t eventMask;
    union {
        struct x11_clientMessage clientMessage;
    };
};

#define x11_grabPointer_OPCODE 26
#define x11_grabPointer_SYNCHRONOUS 0
#define x11_grabPointer_ASYNCHRONOUS 1
struct x11_grabPointer {
    uint8_t opcode;
    uint8_t ownerEvents;
    uint16_t length;
    uint32_t grabWindowId;
    uint16_t eventMask;
    uint8_t pointerMode;
    uint8_t keyboardMode;
    uint32_t confineToWindowId;
    uint32_t cursor;
    uint32_t time;
};

#define x11_ungrabPointer_OPCODE 27
struct x11_ungrabPointer {
    uint8_t opcode;
    uint8_t __pad;
    uint16_t length;
    uint32_t time; // Set to 0 for CurrentTime.
};

#define x11_queryExtension_OPCODE 98
struct x11_queryExtension {
    uint8_t opcode;
    uint8_t __pad;
    uint16_t length;
    uint16_t nameLength;
    uint16_t __pad2;
    uint8_t data[]; // char name[nameLength];
                    // uint8_t __pad3[math_PAD_BYTES(nameLength, 4)];
};

struct x11_queryExtensionResponse {
    uint8_t type;
    uint8_t __pad;
    uint16_t sequenceNumber;
    uint32_t length;
    uint8_t present;
    uint8_t majorOpcode;
    uint8_t firstEvent;
    uint8_t firstError;
    uint8_t __pad2[20];
};

#define x11_getKeyboardMapping_OPCODE 101
struct x11_getKeyboardMapping {
    uint8_t opcode;
    uint8_t __pad;
    uint16_t length;
    uint8_t firstKeycode;
    uint8_t count;
    uint8_t __pad2[2];
};

struct x11_getKeyboardMappingResponse {
    uint8_t type;
    uint8_t keysymsPerKeycode;
    uint16_t sequenceNumber;
    uint32_t length;
    uint8_t __pad[24];
    uint32_t keysyms[];
};

#define x11_getModifierMapping_OPCODE 119
struct x11_getModifierMapping {
    uint8_t opcode;
    uint8_t __pad;
    uint16_t length;
};

struct x11_getModifierMappingResponse {
    uint8_t type;
    uint8_t keycodesPerModifier;
    uint16_t sequenceNumber;
    uint32_t length;
    uint8_t __pad[24];
    uint8_t keycodes[];
};

// XInput2 extension.
#define x11_XINPUT_NAME "XInputExtension"

#define x11_XINPUT_ALL_DEVICES 0
#define x11_XINPUT_ALL_MASTER_DEVICES 1

#define x11_XINPUT_DEVICE_CHANGED 1
#define x11_XINPUT_KEY_PRESS 2
#define x11_XINPUT_KEY_RELEASE 3
#define x11_XINPUT_BUTTON_PRESS 4
#define x11_XINPUT_BUTTON_RELEASE 5
#define x11_XINPUT_MOTION 6
#define x11_XINPUT_ENTER 7
#define x11_XINPUT_LEAVE 8
#define x11_XINPUT_FOCUS_IN 9
#define x11_XINPUT_FOCUS_OUT 10
#define x11_XINPUT_HIERARCHY_CHANGED 11
#define x11_XINPUT_PROPERTY_EVENT 12
#define x11_XINPUT_RAW_KEY_PRESS 13
#define x11_XINPUT_RAW_KEY_RELEASE 14
#define x11_XINPUT_RAW_BUTTON_PRESS 15
#define x11_XINPUT_RAW_BUTTON_RELEASE 16
#define x11_XINPUT_RAW_MOTION 17
#define x11_XINPUT_TOUCH_BEGIN 18 // XI 2.2
#define x11_XINPUT_TOUCH_UPDATE 19
#define x11_XINPUT_TOUCH_END 20
#define x11_XINPUT_TOUCH_OWNERSHIP 21
#define x11_XINPUT_RAW_TOUCH_BEGIN 22
#define x11_XINPUT_RAW_TOUCH_UPDATE 23
#define x11_XINPUT_RAW_TOUCH_END 24
#define x11_XINPUT_BARRIER_HIT 25 // XI 2.3
#define x11_XINPUT_BARRIER_LEAVE 26

struct x11_xinputEventMask {
    uint16_t deviceId;
    uint16_t maskLength; // Always 1.
    uint32_t mask;
};

struct x11_xinputFP3232 {
    int32_t integer;
    uint32_t fraction;
};

#define x11_xinputSelectEvents_OPCODE 46
struct x11_xinputSelectEvents {
    uint8_t majorOpcode;
    uint8_t opcode;
    uint16_t length;
    uint32_t windowId;
    uint16_t numMasks;
    uint16_t __pad;
    struct x11_xinputEventMask masks[];
};

struct x11_xinputRawEvent {
    uint8_t type;
    uint8_t extension;
    uint16_t sequenceNumber;
    uint32_t length;
    uint16_t eventType;

    uint16_t deviceId;
    uint32_t timeMs;
    uint32_t detail;
    uint16_t sourceId;
    uint16_t numValuators;
    uint32_t flags;
    uint32_t __pad;
    uint32_t data[]; // uint32_t valuators[numValuators];
                     // struct x11_xinputFP3232 axisValues[hc_POPCOUNT32(valuators[0]) + ... + hc_POPCOUNT32(valuators[numValuators - 1])];
                     // struct x11_xinputFP3232 axisValuesRaw[hc_POPCOUNT32(valuators[0]) + ... + hc_POPCOUNT32(valuators[numValuators - 1])];
};

// XFixes extension.
#define x11_XFIXES_NAME "XFIXES"

#define x11_xfixesQueryVersion_OPCODE 0
struct x11_xfixesQueryVersion {
    uint8_t majorOpcode;
    uint8_t opcode;
    uint16_t length;
    uint32_t majorVersion;
    uint32_t minorVersion;
};

#define x11_xfixesHideCursor_OPCODE 29
struct x11_xfixesHideCursor {
    uint8_t majorOpcode;
    uint8_t opcode;
    uint16_t length;
    uint32_t windowId;
};

#define x11_xfixesShowCursor_OPCODE 30
struct x11_xfixesShowCursor {
    uint8_t majorOpcode;
    uint8_t opcode;
    uint16_t length;
    uint32_t windowId;
};
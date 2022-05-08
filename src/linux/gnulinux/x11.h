#define x11_TYPE_ERROR 0
#define x11_TYPE_REPLY 1

#define x11_COPY_FROM_PARENT 0
#define x11_INPUT_OUTPUT 1
#define x11_INPUT_ONLY 2

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

// Protocol requests and responses.
#define x11_createWindow_OPCODE 1
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
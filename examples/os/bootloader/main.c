#include "../../../src/hc.h"
#include "../../../src/efi.h"
#include "../../../src/util.c"
#include "../../../src/libc/small.c"
#include "kernelBin.c"

static uint8_t memoryMap[16384];

static void printNum(struct efi_simpleTextOutputProtocol *consoleOut, int64_t number) {
    uint16_t string[util_INT64_MAX_CHARS + 1];
    char *numberStart = util_intToStr((char *)&string[util_UINT64_MAX_CHARS], number);
    int64_t stringLength = (char *)&string[util_INT64_MAX_CHARS] - numberStart;
    util_strToUtf16(&string[0], numberStart, stringLength);
    string[stringLength] = u'\0';
    consoleOut->outputString(consoleOut, &string[0]);
}

static void readKey(struct efi_systemTable *systemTable, struct efi_inputKey *key) {
    uint64_t keyEventIndex;
    systemTable->bootServices->waitForEvent(1, &systemTable->consoleIn->waitForKeyEvent, &keyEventIndex);
    systemTable->consoleIn->readKeyStroke(systemTable->consoleIn, key);
}

static int64_t setupGraphics(struct efi_systemTable *systemTable, struct efi_graphicsOutputProtocol **graphics) {
    uint64_t numHandles;
    void **handleBuffer;
    struct efi_guid graphicsGuid = efi_guid_GRAPHICS_OUTPUT_PROTOCOL;
    int64_t status = systemTable->bootServices->locateHandleBuffer(
        efi_BY_PROTOCOL,
        &graphicsGuid,
        NULL,
        &numHandles,
        &handleBuffer
    );
    if (status < 0) return -1;

    struct efi_graphicsOutputProtocol *graphicsProtocol;
    status = systemTable->bootServices->handleProtocol(
        handleBuffer[0], // Always use first screen.
        &graphicsGuid,
        (void **)&graphicsProtocol
    );
    systemTable->bootServices->freePool(handleBuffer);
    if (status < 0) return -2;

    int32_t bestModeIndex = -1;
    uint64_t bestModeArea = 0;
    for (int32_t i = 0; i < (int32_t)graphicsProtocol->mode->maxMode; ++i) {
        uint64_t infoSize;
        struct efi_graphicsOutputModeInformation *info;
        if (
            graphicsProtocol->queryMode(graphicsProtocol, (uint32_t)i, &infoSize, &info) < 0
        ) continue;

        bool isOk = (
            info->pixelFormat == efi_PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR &&
            info->horizontalResolution == info->pixelsPerScanLine // Don't wanna deal with this weirdness.
        );

        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Index: ");
        printNum(systemTable->consoleOut, i);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u", Width: ");
        printNum(systemTable->consoleOut, info->horizontalResolution);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u", Height: ");
        printNum(systemTable->consoleOut, info->verticalResolution);
        if (isOk) systemTable->consoleOut->outputString(systemTable->consoleOut, u" OK\r\n");
        else      systemTable->consoleOut->outputString(systemTable->consoleOut, u"\r\n");

        uint64_t area = (uint64_t)info->horizontalResolution * (uint64_t)info->verticalResolution;
        if (area > bestModeArea) {
            bestModeIndex = i;
            bestModeArea = area;
        }
    }
    if (bestModeIndex < 0) return -3;

    *graphics = graphicsProtocol;
    return bestModeIndex;
}

int64_t main(void *imageHandle, struct efi_systemTable *systemTable) {
    // Setup graphics.
    struct efi_graphicsOutputProtocol *graphics;
    int64_t status = setupGraphics(systemTable, &graphics);
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed to setup graphics (\r\n");
        printNum(systemTable->consoleOut, status);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u")\r\n");
        return 1;
    }

    // Ask user about screen mode.
    systemTable->consoleOut->outputString(systemTable->consoleOut, u"Press 'a' to use mode ");
    printNum(systemTable->consoleOut, status);
    systemTable->consoleOut->outputString(systemTable->consoleOut, u", any other key to keep current.");
    struct efi_inputKey key;
    readKey(systemTable, &key);
    if (key.unicodeChar == u'a') {
        if (graphics->setMode(graphics, (uint32_t)0) < 0) return 1;
    }

    // Fetch memory map.
    uint64_t memoryMapSize = sizeof(memoryMap);
    uint64_t memoryMapKey;
    uint64_t descriptorSize;
    uint32_t descriptorVersion;
    status = systemTable->bootServices->getMemoryMap(
        &memoryMapSize,
        &memoryMap[0],
        &memoryMapKey,
        &descriptorSize,
        &descriptorVersion
    );
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed to get memory map (\r\n");
        printNum(systemTable->consoleOut, status);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u")\r\n");
        return 1;
    }

    // Exit boot services.
    status = systemTable->bootServices->exitBootServices(imageHandle, memoryMapKey);
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed exit boot services (\r\n");
        printNum(systemTable->consoleOut, status);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u")\r\n");
        return 1;
    }
    // We are on our own!

    // Load kernel into correct memory location.
    hc_MEMCPY((void *)0x100000, &kernelBin[0], kernelBin_size);

    // Call the kernel start function.
    void (hc_SYSV_ABI *start)(struct efi_graphicsOutputProtocol *graphics) = (void *)0x100000;
    start(graphics);
    hc_UNREACHABLE;
}
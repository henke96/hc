#include "../../../src/hc.h"
#include "../../../src/efi.h"
#include "../../../src/util.c"
#include "../../../src/libc/small.c"
#include "../../../src/x86_64/msr.c"
#include "../common/finalPage.c"
#include "kernelBin.c"

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

static int64_t getMemoryMap(struct efi_systemTable *systemTable, uint8_t **memoryMap, uint64_t *memoryMapSize, uint64_t *memoryMapKey, uint64_t *descriptorSize) {
    *memoryMapSize = 0;
    uint32_t descriptorVersion;
    int64_t status = systemTable->bootServices->getMemoryMap(
        memoryMapSize,
        NULL,
        memoryMapKey,
        descriptorSize,
        &descriptorVersion
    );
    if (status != efi_BUFFER_TOO_SMALL) return -1;

    *memoryMapSize += *descriptorSize * 2; // The `allocatePool` call might increase the size.
    status = systemTable->bootServices->allocatePool(efi_LOADER_DATA, *memoryMapSize, (void **)memoryMap);
    if (status < 0) return -2;

    status = systemTable->bootServices->getMemoryMap(
        memoryMapSize,
        *memoryMap,
        memoryMapKey,
        descriptorSize,
        &descriptorVersion
    );
    if (status < 0) return -3;
    return 0;
}

int64_t main(void *imageHandle, struct efi_systemTable *systemTable) {
    // Setup graphics.
    struct efi_graphicsOutputProtocol *graphics;
    int64_t status = setupGraphics(systemTable, &graphics);
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed to setup graphics (");
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
        if (graphics->setMode(graphics, (uint32_t)status) < 0) return 1;
    }

    // Get memory map.
    uint8_t *memoryMap;
    uint64_t memoryMapSize;
    uint64_t memoryMapKey;
    uint64_t descriptorSize;
    status = getMemoryMap(systemTable, &memoryMap, &memoryMapSize, &memoryMapKey, &descriptorSize);
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed to get memory map (");
        printNum(systemTable->consoleOut, status);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u")\r\n");
        return 1;
    }

    // Make sure there's enough available memory for the reserved pages.
    if (finalPage_getReservedPageAddress(memoryMap, memoryMapSize, descriptorSize, 2) < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Not enough available memory!\r\n");
        return 1;
    }

    // Make sure the memory map will fit in the final page.
    uint64_t memoryMapLength = memoryMapSize / descriptorSize;
    if (memoryMapLength * sizeof(struct efi_memoryDescriptor) > 0x200000u - offsetof(struct finalPage, memoryMap)) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Memory map too big!\r\n");
        return 1;
    }

    // Exit boot services.
    status = systemTable->bootServices->exitBootServices(imageHandle, memoryMapKey);
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed exit boot services (");
        printNum(systemTable->consoleOut, status);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u")\r\n");
        return 1;
    }
    // We are on our own!

    uint64_t finalPageAddress = (uint64_t)finalPage_getReservedPageAddress(memoryMap, memoryMapSize, descriptorSize, 1);
    uint64_t kernelAddress = (uint64_t)finalPage_getReservedPageAddress(memoryMap, memoryMapSize, descriptorSize, 2);

    // Load kernel where we want it in physical memory.
    hc_MEMCPY((void *)kernelAddress, &kernelBin[0], kernelBin_size);

    // Fill out contents of final page.
    struct finalPage *finalPage = (void *)finalPageAddress;
    finalPage->frameBufferWidth = graphics->mode->info->horizontalResolution;
    finalPage->frameBufferHeight = graphics->mode->info->verticalResolution;
    finalPage->frameBufferBase = graphics->mode->frameBufferBase;
    finalPage->memoryMapLength = memoryMapLength;
    for (uint64_t i = 0; i < memoryMapLength; ++i) {
        struct efi_memoryDescriptor *descriptor = (void *)&memoryMap[descriptorSize * i];
        finalPage->memoryMap[i] = *descriptor;
    }
    // Create page tables.
    finalPage->pageTableL4 = (uint64_t)&finalPage->pageTableL3 | 0b11;
    for (uint64_t i = 0; i < 4; ++i) {
        finalPage->pageTableL3[i] = (uint64_t)&finalPage->pageTableL2[i * 512] | 0b11;
    }
    hc_MEMSET(&finalPage->pageTableL2, 0, 4 * 512 * sizeof(finalPage->pageTableL2[0]));
    finalPage->pageTableL2[0] = kernelAddress | 0b10000011;
    finalPage->pageTableL2[finalPage_VIRTUAL_ADDRESS / 0x200000u] = finalPageAddress | 0b10000011;
    // Identity map the page containing the kernel start code, since that's where we enable paging.
    finalPage->pageTableL2[kernelAddress / 0x200000u] = kernelAddress | 0b10000011;

    // Jump to kernel start.
    asm volatile(
        "mov %0, %%rcx\n"
        "jmp *%1\n"
        :
        : "r"(finalPageAddress + offsetof(struct finalPage, pageTableL4)), "r"(kernelAddress)
        : "rcx", "memory"
    );
    hc_UNREACHABLE;
}
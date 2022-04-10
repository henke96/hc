#include "../../src/hc.h"
#include "../../src/efi.h"
#include "../../src/util.c"

static uint8_t memoryMap[65536];

static void printNum(struct efi_simpleTextOutputProtocol *consoleOut, int64_t number) {
    uint16_t string[util_INT64_MAX_CHARS + 1];
    char *numberStart = util_intToStr((char *)&string[util_UINT64_MAX_CHARS], number);
    int64_t stringLength = (char *)&string[util_INT64_MAX_CHARS] - numberStart;
    util_strToUtf16(&string[0], numberStart, stringLength);
    string[stringLength] = u'\0';
    consoleOut->outputString(consoleOut, &string[0]);
}

static void printMemoryType(struct efi_simpleTextOutputProtocol *consoleOut, enum efi_memoryType type) {
    const uint16_t *string = u"Unknown";
    switch (type) {
        case efi_RESERVED_MEMORY_TYPE:        string = u"Reserved"; break;
        case efi_LOADER_CODE:                 string = u"LoaderCode"; break;
        case efi_LOADER_DATA:                 string = u"LoaderData"; break;
        case efi_BOOT_SERVICES_CODE:          string = u"BootServicesCode"; break;
        case efi_BOOT_SERVICES_DATA:          string = u"BootServicesData"; break;
        case efi_RUNTIME_SERVICES_CODE:       string = u"RuntimeServicesCode"; break;
        case efi_RUNTIME_SERVICES_DATA:       string = u"RuntimeServicesData"; break;
        case efi_CONVENTIONAL_MEMORY:         string = u"Conventional"; break;
        case efi_UNUSABLE_MEMORY:             string = u"Unusable"; break;
        case efi_ACPI_RECLAIM_MEMORY:         string = u"AcpiReclaim"; break;
        case efi_ACPI_MEMORY_NVS:             string = u"AcpiNvs"; break;
        case efi_MEMORY_MAPPED_IO:            string = u"MemoryMappedIo"; break;
        case efi_MEMORY_MAPPED_IO_PORT_SPACE: string = u"MemoryMappedIoPortSpace"; break;
        case efi_PAL_CODE:                    string = u"PalCode"; break;
        case efi_PERSISTENT_MEMORY:           string = u"Persistent"; break;
        default: break;
    }
    consoleOut->outputString(consoleOut, &string[0]);
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

    int32_t selectedModeIndex = -1;
    uint32_t selectedModeWidth = 0;
    for (int32_t i = 0; i < (int32_t)graphicsProtocol->mode->maxMode; ++i) {
        uint64_t infoSize;
        struct efi_graphicsOutputModeInformation *info;
        if (
            graphicsProtocol->queryMode(graphicsProtocol, (uint32_t)i, &infoSize, &info) < 0
        ) continue;
        if (info->pixelFormat != efi_PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR) continue;
        if (graphicsProtocol->mode->info->horizontalResolution > selectedModeWidth) {
            selectedModeIndex = i;
            selectedModeWidth = graphicsProtocol->mode->info->horizontalResolution;
        }
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Index: ");
        printNum(systemTable->consoleOut, i);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u", Width: ");
        printNum(systemTable->consoleOut, graphicsProtocol->mode->info->horizontalResolution);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u", Height: ");
        printNum(systemTable->consoleOut, graphicsProtocol->mode->info->verticalResolution);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"\r\n");
    }
    if (selectedModeIndex < 0) return -3;


    systemTable->consoleOut->outputString(systemTable->consoleOut, u"Selected: ");
    printNum(systemTable->consoleOut, selectedModeIndex);
    systemTable->consoleOut->outputString(systemTable->consoleOut, u"\r\n");
    status = graphicsProtocol->setMode(graphicsProtocol, (uint32_t)selectedModeIndex);
    if (status < 0) return -4;

    *graphics = graphicsProtocol;
    return 0;
}

int64_t main(hc_UNUSED void *imageHandle, struct efi_systemTable *systemTable) {
    uint64_t memoryMapSize = sizeof(memoryMap);
    uint64_t memoryMapKey;
    uint64_t descriptorSize;
    uint32_t descriptorVersion;
    int64_t status = systemTable->bootServices->getMemoryMap(
        &memoryMapSize,
        &memoryMap[0],
        &memoryMapKey,
        &descriptorSize,
        &descriptorVersion
    );
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed to get memory map\r\n");
        return 1;
    }

    systemTable->consoleOut->outputString(systemTable->consoleOut, u"Memory map:\r\n");
    for (uint64_t offset = 0; offset < memoryMapSize; offset += descriptorSize) {
        struct efi_memoryDescriptor *descriptor = (struct efi_memoryDescriptor *)&memoryMap[offset];
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Type: ");
        printMemoryType(systemTable->consoleOut, descriptor->type);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u", Physical start: ");
        printNum(systemTable->consoleOut, (int64_t)descriptor->physicalStart);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u", Pages: ");
        printNum(systemTable->consoleOut, (int64_t)descriptor->numberOfPages);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"\r\n");
    }

    struct efi_graphicsOutputProtocol *graphics;
    status = setupGraphics(systemTable, &graphics);
    if (status < 0) {
        systemTable->consoleOut->outputString(systemTable->consoleOut, u"Failed to setup graphics (\r\n");
        printNum(systemTable->consoleOut, status);
        systemTable->consoleOut->outputString(systemTable->consoleOut, u")\r\n");
        return 1;
    }

    // Do some drawing.
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    for (;;) {
        uint32_t colour = (red << 16) | (green << 8) | blue;
        for (uint64_t y = 0; y < graphics->mode->info->verticalResolution; ++y) {
            uint32_t *base = &((uint32_t *)graphics->mode->frameBufferBase)[y * graphics->mode->info->pixelsPerScanLine];
            for (uint64_t x = 0; x < graphics->mode->info->horizontalResolution; ++x) {
                base[x] = colour;
            }
        }
        // Continuous iteration of colours.
        if (red == 0 && green == 0 && blue != 255) ++blue;
        else if (red == 0 && green != 255 && blue == 255) ++green;
        else if (red == 0 && green == 255 && blue != 0) --blue;
        else if (red != 255 && green == 255 && blue == 0) ++red;
        else if (red == 255 && green == 255 && blue != 255) ++blue;
        else if (red == 255 && green != 0 && blue == 255) --green;
        else if (red == 255 && green == 0 && blue != 0) --blue;
        else --red;
    }
    return 0;
}
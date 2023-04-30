// Contents of the 2 MiB page set up by bootloader.
struct bootloaderPage {
    // All page tables need 4096 bytes alignment.
    uint64_t pageTableL2[4 * 512]; // Support 4 GiB of virtual memory.
    uint64_t pageTableL3[512];
    uint64_t pageTableL4;
    uint32_t frameBufferWidth;
    uint32_t frameBufferHeight;
    uint64_t frameBufferBase;
    uint64_t memoryMapLength;
    struct efi_memoryDescriptor memoryMap[];
};

// Get the physical address of the first free 2 MiB page at or after `startPageAddress`.
static int64_t bootloaderPage_getFreePageAddress(void *memoryMap, uint64_t memoryMapSize, uint64_t descriptorSize, uint64_t startPageAddress) {
    for (uint64_t mapOffset = 0; mapOffset < memoryMapSize; mapOffset += descriptorSize) {
        struct efi_memoryDescriptor *descriptor = (void *)&memoryMap[mapOffset];
        if (descriptor->type != efi_CONVENTIONAL_MEMORY) continue;

        uint64_t alignedStart = math_ALIGN_FORWARD(descriptor->physicalStart, paging_PAGE_SIZE);
        if (alignedStart < startPageAddress) alignedStart = startPageAddress;
        uint64_t end = descriptor->physicalStart + descriptor->numberOfPages * 4096;

        if (end >= alignedStart + paging_PAGE_SIZE) return (int64_t)alignedStart;
    }
    return -1;
}

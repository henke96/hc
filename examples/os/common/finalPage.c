#define finalPage_VIRTUAL_ADDRESS 0xffe00000u

// Contents of the final 2 MiB page table entry.
struct finalPage {
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

// Get the physical address of the nth bootloader reserved 2 MiB page.
static int64_t finalPage_getReservedPageAddress(void *memoryMap, uint64_t memoryMapSize, uint64_t descriptorSize, uint32_t n) {
    for (uint64_t mapOffset = 0; mapOffset < memoryMapSize; mapOffset += descriptorSize) {
        struct efi_memoryDescriptor *descriptor = (void *)&memoryMap[mapOffset];
        if (descriptor->type != efi_CONVENTIONAL_MEMORY) continue;

        uint64_t alignedStart = util_ALIGN_FORWARD(descriptor->physicalStart, 0x200000u);
        if (alignedStart >= finalPage_VIRTUAL_ADDRESS) return -1; // We want it below the final page to allow identity mapping.
        uint64_t end = descriptor->physicalStart + descriptor->numberOfPages * 4096;
        if (end < alignedStart) continue;

        uint64_t num2MibPages = (end - alignedStart) / 0x200000;
        if (num2MibPages >= n) return (int64_t)(alignedStart + (n - 1) * 0x200000);
        n -= num2MibPages;
    }
    return -2;
}
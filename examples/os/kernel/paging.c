static void paging_init(void) {
    // Setup PAT.
    uint64_t pat = (
        (msr_MEM_TYPE_WB       << 0 * 8) |
        (msr_MEM_TYPE_WT       << 1 * 8) |
        (msr_MEM_TYPE_UC_MINUS << 2 * 8) |
        (msr_MEM_TYPE_UC       << 3 * 8) |
        (msr_MEM_TYPE_WB       << 4 * 8) |
        (msr_MEM_TYPE_WT       << 5 * 8) |
        (msr_MEM_TYPE_UC_MINUS << 6 * 8) |
        (msr_MEM_TYPE_WC       << 7 * 8)
    );
    msr_wrmsr(msr_PAT, pat);

    struct bootloaderPage *bootloaderPage = (void *)paging_BOOTLOADER_PAGE_ADDRESS;
    // Use same logic as bootloader to get same physical addresses.
    uint64_t bootloaderPagePhysicalAddress = (uint64_t)bootloaderPage_getFreePageAddress(
        &bootloaderPage->memoryMap,
        bootloaderPage->memoryMapLength * sizeof(bootloaderPage->memoryMap[0]),
        sizeof(bootloaderPage->memoryMap[0]),
        0
    );
    uint64_t kernelPagePhysicalAddress = (uint64_t)bootloaderPage_getFreePageAddress(
        &bootloaderPage->memoryMap,
        bootloaderPage->memoryMapLength * sizeof(bootloaderPage->memoryMap[0]),
        sizeof(bootloaderPage->memoryMap[0]),
        bootloaderPagePhysicalAddress + paging_PAGE_SIZE
    );

    // Remove the temporary identity mapping of kernel start code.
    bootloaderPage->pageTableL2[kernelPagePhysicalAddress >> paging_PAGE_SHIFT] = 0;

    // Map the frame buffer.
    uint64_t frameBufferMapStart = bootloaderPage->frameBufferBase & paging_PAGE_MASK;
    uint64_t frameBufferSize = sizeof(uint32_t) * bootloaderPage->frameBufferWidth * bootloaderPage->frameBufferHeight;
    uint64_t frameBufferMapEnd = math_ALIGN_FORWARD(bootloaderPage->frameBufferBase + frameBufferSize, paging_PAGE_SIZE);
    uint64_t numPages = (frameBufferMapEnd - frameBufferMapStart) >> paging_PAGE_SHIFT;
    for (uint64_t i = 0; i < numPages; ++i) {
        // Enable write-combine for framebuffer pages.
        bootloaderPage->pageTableL2[(paging_FRAMEBUFFER_ADDRESS >> paging_PAGE_SHIFT) + i] = (frameBufferMapStart + (i << paging_PAGE_SHIFT)) | 0b1000010011011;
    }

    asm volatile(
        "mov %0, %%cr3\n"
        :
        : "r"(bootloaderPagePhysicalAddress + offsetof(struct bootloaderPage, pageTableL4))
        : "memory"
    );
}

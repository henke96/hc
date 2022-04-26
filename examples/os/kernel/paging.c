#define paging_PAGE_SIZE 0x200000u
#define paging_FRAMEBUFFER_ADDRESS 0x80000000u

static void paging_init(void) {
    // Setup PAT.
    uint64_t pat = (
        ((uint64_t)msr_MEM_TYPE_WB       << 0 * 8) |
        ((uint64_t)msr_MEM_TYPE_WT       << 1 * 8) |
        ((uint64_t)msr_MEM_TYPE_UC_MINUS << 2 * 8) |
        ((uint64_t)msr_MEM_TYPE_UC       << 3 * 8) |
        ((uint64_t)msr_MEM_TYPE_WB       << 4 * 8) |
        ((uint64_t)msr_MEM_TYPE_WT       << 5 * 8) |
        ((uint64_t)msr_MEM_TYPE_UC_MINUS << 6 * 8) |
        ((uint64_t)msr_MEM_TYPE_WC       << 7 * 8)
    );
    msr_wrmsr(msr_PAT, pat);

    struct finalPage *finalPage = (void *)finalPage_VIRTUAL_ADDRESS;
    uint64_t finalPagePhysicalAddress = (uint64_t)finalPage_getReservedPageAddress(
        &finalPage->memoryMap,
        finalPage->memoryMapLength * sizeof(finalPage->memoryMap[0]),
        sizeof(finalPage->memoryMap[0]),
        1
    );
    uint64_t kernelPhysicalAddress = (uint64_t)finalPage_getReservedPageAddress(
        &finalPage->memoryMap,
        finalPage->memoryMapLength * sizeof(finalPage->memoryMap[0]),
        sizeof(finalPage->memoryMap[0]),
        2
    );

    // Remove the temporary identity mapping of kernel start code.
    finalPage->pageTableL2[kernelPhysicalAddress / paging_PAGE_SIZE] = 0;

    // Map the frame buffer.
    uint64_t frameBufferMapStart = finalPage->frameBufferBase & ~(paging_PAGE_SIZE - 1);
    uint64_t frameBufferSize = sizeof(uint32_t) * finalPage->frameBufferWidth * finalPage->frameBufferHeight;
    uint64_t frameBufferMapEnd = util_ALIGN_FORWARD(finalPage->frameBufferBase + frameBufferSize, paging_PAGE_SIZE);
    uint64_t numPages = (frameBufferMapEnd - frameBufferMapStart) / paging_PAGE_SIZE;
    for (uint64_t i = 0; i < numPages; ++i) {
        // Enable write-combine for framebuffer pages.
        finalPage->pageTableL2[(paging_FRAMEBUFFER_ADDRESS / paging_PAGE_SIZE) + i] = (frameBufferMapStart + i * paging_PAGE_SIZE) | 0b1000010011011;
    }

    asm volatile(
        "mov %0, %%cr3\n"
        :
        : "r"(finalPagePhysicalAddress + offsetof(struct finalPage, pageTableL4))
        : "memory"
    );
}
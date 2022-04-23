#include "../../../src/hc.h"
#include "../../../src/efi.h"
#include "../../../src/util.c"
#include "../../../src/libc/musl.c"
#include "../../../src/x86_64/msr.c"
#include "../common/finalPage.c"

extern uint8_t kernel_bssStart;
extern uint8_t kernel_bssEnd;
extern uint8_t kernel_stackTop;
extern uint8_t kernel_stackBottom;

// Section for uninitialized data. Like `.bss` but doesn't get zeroed at runtime.
asm(".section .uninit,\"aw\",@nobits\n");

// Entry point.
asm(
    ".section .start,\"ax\",@progbits\n"
    // Apply the page tables set up by bootloader (address in rcx).
    "mov %rcx, %cr3\n"
    // Jump from the temporary identity mapping to the real mapping.
    "mov $start, %rax\n"
    "jmp *%rax\n"
    "start:\n"
    // Clear direction and interrupt flags.
    "cld\n"
    "cli\n"
    // Clear segment registers.
    "xor %eax, %eax\n"
    "mov %eax, %ds\n"
    "mov %eax, %es\n"
    "mov %eax, %ss\n"
    "mov %eax, %fs\n"
    "mov %eax, %gs\n"
    // Clear bss segment.
    "lea kernel_bssStart(%rip), %rdi\n"
    "lea kernel_bssEnd(%rip), %rcx\n"
    "sub %rdi, %rcx\n"
    "shr $3, %rcx\n"
    "rep stosq\n"
    // Setup stack.
    "lea kernel_stackTop(%rip), %esp\n"
    // Call the main function.
    "call main\n"
);

#define FRAMEBUFFER_VIRTUAL_ADDRESS 0x80000000

static void initPaging(void) {
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
    finalPage->pageTableL2[kernelPhysicalAddress / 0x200000u] = 0;

    // Map the frame buffer.
    uint64_t frameBufferMapStart = finalPage->frameBufferBase & ~(0x200000u - 1);
    uint64_t frameBufferSize = sizeof(uint32_t) * finalPage->frameBufferWidth * finalPage->frameBufferHeight;
    uint64_t frameBufferMapEnd = util_ALIGN_FORWARD(finalPage->frameBufferBase + frameBufferSize, 0x200000u);
    uint64_t numPages = (frameBufferMapEnd - frameBufferMapStart) / 0x200000u;
    for (uint64_t i = 0; i < numPages; ++i) {
        // Enable write-combine for framebuffer pages.
        finalPage->pageTableL2[(FRAMEBUFFER_VIRTUAL_ADDRESS / 0x200000u) + i] = (frameBufferMapStart + i * 0x200000u) | 0b1000010011011;
    }

    asm volatile(
        "mov %0, %%cr3\n"
        :
        : "r"(finalPagePhysicalAddress + offsetof(struct finalPage, pageTableL4))
        : "memory"
    );
}

void noreturn main(void) {
    initPaging();

    struct finalPage *finalPage = (void *)finalPage_VIRTUAL_ADDRESS;
    uint32_t numPixels = finalPage->frameBufferWidth * finalPage->frameBufferHeight;

    // Do some drawing.
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    for (;;) {
        uint32_t colour = (red << 16) | (green << 8) | blue;
        for (uint32_t i = 0; i < numPixels; ++i) ((uint32_t *)FRAMEBUFFER_VIRTUAL_ADDRESS)[i] = colour;

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
}

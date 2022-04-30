#include "../../../src/hc.h"
#include "../../../src/efi.h"
#include "../../../src/util.c"
#include "../../../src/libc/musl.c"
#include "../../../src/x86_64/msr.c"
#include "../common/paging.h"
#include "../common/bootloaderPage.c"
#include "paging.c"

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

void noreturn main(void) {
    paging_init();

    struct bootloaderPage *bootloaderPage = (void *)paging_BOOTLOADER_PAGE_ADDRESS;
    uint32_t numPixels = bootloaderPage->frameBufferWidth * bootloaderPage->frameBufferHeight;

    // Do some drawing.
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    for (;;) {
        uint32_t colour = (red << 16) | (green << 8) | blue;
        for (uint32_t i = 0; i < numPixels; ++i) ((uint32_t *)paging_FRAMEBUFFER_ADDRESS)[i] = colour;

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

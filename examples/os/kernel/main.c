#include "../../../src/hc.h"
#include "../../../src/efi.h"
#include "../../../src/util.c"
#include "../../../src/libc/musl.c"
#include "x86.c"

extern uint8_t kernel_bssStart;
extern uint8_t kernel_bssEnd;
extern uint8_t kernel_stackTop;
extern uint8_t kernel_stackBottom;

// Section for uninitialized data. Like `.bss` but doesn't get zeroed at runtime.
asm(".section .uninit,\"aw\",@nobits\n");

// Entry point.
asm(
    ".section .start,\"ax\",@progbits\n"
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
    "mov %rdi, %r8\n"
    "lea kernel_bssStart(%rip), %rdi\n"
    "lea kernel_bssEnd(%rip), %rcx\n"
    "sub %rdi, %rcx\n"
    "shr $3, %rcx\n"
    "rep stosq\n"
    "mov %r8, %rdi\n"
    // Setup stack.
    "lea kernel_stackTop(%rip), %esp\n"
    // Call the main function.
    "call main\n"
);

#define IDENTIY_MAP_SIZE_GIB 32
hc_SECTION(".uninit")
hc_ALIGNED(4096)
uint64_t pageMapL4;
hc_SECTION(".uninit")
hc_ALIGNED(0x200000)
static uint64_t pageMapL3[IDENTIY_MAP_SIZE_GIB];
hc_SECTION(".uninit")
hc_ALIGNED(4096)
static uint64_t pageMapL2[IDENTIY_MAP_SIZE_GIB * 512];

static void initPaging(uint64_t frameBufferAddr, uint64_t frameBufferSize) {
    uint64_t pat = x86_rdmsr(0x277);
    uint8_t *entries = (uint8_t *)&pat;
    entries[7] = 0x01; // Write-combine
    x86_wrmsr(0x277, pat);

    pageMapL4 = (uint64_t)&pageMapL3 | 0b11;
    for (uint64_t i = 0; i < IDENTIY_MAP_SIZE_GIB; ++i) {
        pageMapL3[i] = (uint64_t)&pageMapL2[i * 512] | 0b11;
    }
    for (uint64_t i = 0; i < IDENTIY_MAP_SIZE_GIB * 512; ++i) {
        uint64_t base = 0x200000 * i;
        if (util_RANGES_OVERLAP(base, base + 0x200000, frameBufferAddr, frameBufferAddr + frameBufferSize)) {
            pageMapL2[i] = base | 0b1000010011011; // Enable write-combine for framebuffer pages.
        } else {
            pageMapL2[i] = base | 0b10000011;
        }
    }
    asm volatile(
        "lea pageMapL4(%%rip), %%rax\n"
        "mov %%rax, %%cr3\n"
        ::: "rax", "memory"
    );
}

void noreturn main(struct efi_graphicsOutputProtocol *graphics) {
    int32_t numPixels = (int32_t)(graphics->mode->info->verticalResolution * graphics->mode->info->horizontalResolution);
    uint32_t *frameBuffer = (uint32_t *)graphics->mode->frameBufferBase;

    initPaging((uint64_t)frameBuffer, (uint64_t)numPixels * sizeof(frameBuffer[0]));

    // Do some drawing.
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    for (;;) {
        uint32_t colour = (red << 16) | (green << 8) | blue;
        for (int32_t i = 0; i < numPixels; ++i) frameBuffer[i] = colour;

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

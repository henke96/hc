extern const uint8_t kernelBin[];
extern uint32_t kernelBin_size;

#if hc_DEBUG
asm(
    ".section .rdata\n"
    ".global kernelBin\n"
    ".align 8\n"
    "kernelBin:\n"
    ".incbin \"debug_kernel.bin\"\n"
    "kernelBin_end:\n"
    ".global kernelBin_size\n"
    ".align 4\n"
    "kernelBin_size:\n"
    ".int kernelBin_end - kernelBin\n"
);
#else
asm(
    ".section .rdata\n"
    ".global kernelBin\n"
    ".align 8\n"
    "kernelBin:\n"
    ".incbin \"kernel.bin\"\n"
    "kernelBin_end:\n"
    ".global kernelBin_size\n"
    ".align 4\n"
    "kernelBin_size:\n"
    ".int kernelBin_end - kernelBin\n"
);
#endif

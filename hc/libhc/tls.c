// Thread local storage helpers.

hc_UNUSED
static struct elf_programHeader *tls_findProgramHeader(const uint64_t *auxv) {
    struct elf_programHeader *programHeaders = NULL;
    uint64_t programHeadersLength = 0;
    for (int32_t i = 0; auxv[i] != AT_NULL; i += 2) {
        switch (auxv[i]) {
            case AT_PHDR: {
                programHeaders = (struct elf_programHeader *)auxv[i + 1];
                if (programHeadersLength > 0) goto foundProgramHeaders;
                break;
            }
            case AT_PHNUM: {
                programHeadersLength = auxv[i + 1];
                if (programHeaders != NULL) goto foundProgramHeaders;
                break;
            }
        }
    }
    return NULL;

    foundProgramHeaders:
    for (uint64_t i = 0; i < programHeadersLength; ++i) {
        if (programHeaders[i].type == elf_PT_TLS) return &programHeaders[i];
    }
    return NULL;
}

// Initialise a tls area.
// `tlsArea` must have an alignment of at least `tlsProgramHeader->segmentAlignment` and must be
// at least `tlsProgramHeader->segmentMemorySize` bytes long. Bytes at `tlsProgramHeader->segmentFileSize` and after must be zero.
// Returns the thread pointer for the area.
hc_UNUSED static uint64_t tls_initArea(struct elf_programHeader *tlsProgramHeader, void *tlsArea) {
    hc_MEMCPY(tlsArea, tlsProgramHeader->virtualAddress, tlsProgramHeader->segmentFileSize);
#if hc_X86_64
    return (uint64_t)tlsArea + hc_ALIGN_FORWARD(tlsProgramHeader->segmentMemorySize, tlsProgramHeader->segmentAlignment);
#elif hc_AARCH64
    return (uint64_t)tlsArea - 16;
#elif hc_RISCV
    return (uint64_t)tlsArea;
#endif
}

// Set thread pointer for current thread.
hc_UNUSED static hc_ALWAYS_INLINE void tls_setThreadPointer(uint64_t threadPointer) {
#if hc_X86_64
    hc_SYSCALL2(hc_NR_arch_prctl, ARCH_SET_FS, threadPointer)
#elif hc_AARCH64
    asm volatile(
        "msr tpidr_el0, %0\n"
        :
        : "r"(threadPointer)
    );
#elif hc_RISCV
    asm volatile(
        "mv tp, %0\n"
        :
        : "r"(threadPointer)
    );
#endif
}
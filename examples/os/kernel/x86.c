static hc_ALWAYS_INLINE uint64_t x86_rdmsr(uint32_t msrId) {
    register uint32_t id asm("ecx") = msrId;
    register uint32_t low asm("eax");
    register uint32_t high asm("edx");
    asm volatile(
        "rdmsr\n"
        : "=r"(low), "=r"(high)
        : "r"(id)
    );
    return (uint64_t)low | ((uint64_t)high << 32);
}

static hc_ALWAYS_INLINE void x86_wrmsr(uint32_t msrId, uint64_t value) {
    register uint32_t id asm("ecx") = msrId;
    register uint32_t low asm("eax") = (uint32_t)value;
    register uint32_t high asm("edx") = (uint32_t)(value >> 32);
    asm volatile(
        "wrmsr\n"
        :: "r"(id), "r"(low), "r"(high)
        : "memory"
    );
}

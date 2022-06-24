// MSR ids.
#define msr_FS_BASE 0xc0000100u
#define msr_GS_BASE 0xc0000101u
#define msr_KERNEL_GS_BASE 0xc0000102u
#define msr_PAT 0x00000277u

// MTRR and PAT memory types.
#define msr_MEM_TYPE_UC 0x0u
#define msr_MEM_TYPE_WC 0x1u
#define msr_MEM_TYPE_WT 0x4u
#define msr_MEM_TYPE_WP 0x5u
#define msr_MEM_TYPE_WB 0x6u
#define msr_MEM_TYPE_UC_MINUS 0x7u // PAT only.

static hc_ALWAYS_INLINE uint64_t msr_rdmsr(uint32_t msrId) {
    register uint32_t id asm("ecx") = msrId;
    register uint32_t low asm("eax");
    register uint32_t high asm("edx");
    asm volatile(
        "rdmsr\n"
        : "=r"(low), "=r"(high)
        : "r"(id)
    );
    return ((uint64_t)high << 32) | (uint64_t)low;
}

static hc_ALWAYS_INLINE void msr_wrmsr(uint32_t msrId, uint64_t value) {
    register uint32_t id asm("ecx") = msrId;
    register uint32_t low asm("eax") = (uint32_t)value;
    register uint32_t high asm("edx") = (uint32_t)(value >> 32);
    asm volatile(
        "wrmsr\n"
        :: "r"(id), "r"(low), "r"(high)
        : "memory"
    );
}

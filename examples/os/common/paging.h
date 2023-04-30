#define paging_PAGE_SIZE 0x200000u

// NOTE: With the X32 ABI, clang 14 miscompiles stores to addresses at 0x80000000 and above, so we avoid those for now.
// See: https://github.com/llvm/llvm-project/issues/55061

// Virtual address static allocation.
#define paging_KERNEL_ADDRESS 0x0u
#define paging_FRAMEBUFFER_ADDRESS 0x40000000u
#define paging_BOOTLOADER_PAGE_ADDRESS (0x80000000u - paging_PAGE_SIZE)

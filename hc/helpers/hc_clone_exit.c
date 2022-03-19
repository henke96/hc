// Doesn't return in parent if successful, else returns negative error code. Returns 0 in child.
// Safe to use with CLONE_VM.
int32_t hc_clone_exit(struct clone_args *args, uint64_t size);
#if hc_X86_64
asm(
    ".section .text\n"
    ".local hc_clone_exit\n"
    ".type hc_clone_exit, @function\n"
    "hc_clone_exit:\n"
    "mov %rcx, %r10\n"
    "mov $435, %eax\n"
    "syscall\n"
    "test %eax, %eax\n"
    "jle 1f\n"
    "xor %edi, %edi\n"
    "mov $231, %eax\n"
    "syscall\n"
    "1: ret\n"
);
#elif hc_AARCH64
asm(
    ".section .text\n"
    ".local hc_clone_exit\n"
    ".type hc_clone_exit, @function\n"
    "hc_clone_exit:\n"
    "mov x8, 435\n"
    "svc 0\n"
    "cmp x0, 0\n"
    "ble 1f\n"
    "mov x0, 0\n"
    "mov x8, 94\n"
    "svc 0\n"
    "1: ret\n"
);
#endif
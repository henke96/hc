// Returns pid, or negative error code.
int32_t hc_clone(struct clone_args *args, uint64_t size, void (*childfunc)(void *), void *childarg);
#if hc_X86_64
asm(
    ".section .text\n"
    ".local hc_clone\n"
    ".type hc_clone, @function\n"
    "hc_clone:\n"
    "mov %rcx, %r10\n"
    "mov $435, %eax\n"
    "syscall\n"
    "test %eax, %eax\n"
    "jnz 1f\n"
    "xor %ebp, %ebp\n"
    "mov %r10, %rdi\n"
    "call *%rdx\n"
    "1: ret\n"
);
#elif hc_AARCH64
asm(
    ".section .text\n"
    ".local hc_clone\n"
    ".type hc_clone, @function\n"
    "hc_clone:\n"
    "mov x8, 435\n"
    "svc 0\n"
    "cbz x0, 1f\n"
    "ret\n"
    "1: mov x0, x3\n"
    "blr x2\n"
);
#endif

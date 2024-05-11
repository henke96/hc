// Returns pid, or negative error code.
int32_t sys_clone_func(uint32_t flags, void *stackHigh, int32_t *parent_tid, uint64_t tls, int32_t *child_tid, void (*childfunc)(void *), void *childarg);
#if hc_X86_64
asm(
    ".section .text\n"
    ".local sys_clone_func\n"
    "sys_clone_func:\n"
    "push %r12\n" // Save r12.
    "mov 16(%rsp), %r12\n" // Save `childarg` to r12.
    "mov %r8, %r10\n"
    "mov %rcx, %r8\n"
    "mov $56, %eax\n"
    "syscall\n"
    "test %eax, %eax\n"
    "jz 1f\n"
    "pop %r12\n" // Restore r12.
    "ret\n"
    "1: mov %r12, %rdi\n"
    "call *%r9\n"
);
#elif hc_AARCH64
asm(
    ".section .text\n"
    ".local sys_clone_func\n"
    "sys_clone_func:\n"
    "mov x8, 220\n"
    "svc 0\n"
    "cbz x0, 1f\n"
    "ret\n"
    "1: mov x0, x6\n"
    "blr x5\n"
);
#elif hc_RISCV64
asm(
    ".section .text\n"
    ".local sys_clone_func\n"
    "sys_clone_func:\n"
    "li a7, 220\n"
    "ecall\n"
    "beq a0, x0, 1f\n"
    "ret\n"
    "1: mv a0, a6\n"
    "jalr a5\n"
);
#endif

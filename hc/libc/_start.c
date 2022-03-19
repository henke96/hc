#if !hc_LIBC
#if hc_X86_64
asm(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "pop %rdi\n"       // argc -> rdi
    "mov %rsp, %rsi\n" // argv -> rsi
    "and $-16, %rsp\n" // Make sure stack is 16 byte aligned.
    "call main\n"
    "mov %eax, %edi\n"
    "mov $231, %eax\n"
    "syscall\n"        // Run exit_group with the return from main.
);
#elif hc_AARCH64
asm(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "ldr x0, [sp]\n"    // argc -> x0
    "add x1, sp, 8\n"   // argv -> x1
    "and sp, x1, -16\n" // Make sure stack is 16 byte aligned.
    "bl main\n"
    "mov x8, 94\n"
    "svc 0\n"           // Run exit_group with the return from main.
);
#endif
#endif

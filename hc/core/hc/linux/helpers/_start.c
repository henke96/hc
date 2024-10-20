#ifndef _start_FUNC
    #define _start_FUNC start
#endif

int32_t _start_FUNC(int32_t argc, char **argv, char **envp);

#if hc_X86_64
asm(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "pop %rdi\n"                   // argc -> rdi
    "mov %rsp, %rsi\n"             // argv -> rsi
    "lea 8(%rsp, %rdi, 8), %rdx\n" // envp -> rdx
    "and $-16, %rsp\n"             // Make sure stack is 16 byte aligned.
    "call " hc_XSTR(_start_FUNC) "\n"
    "mov %eax, %edi\n"
    "mov $231, %eax\n"
    "syscall\n"                    // Run exit_group with the return from start.
);
#elif hc_AARCH64
asm(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "ldr x0, [sp]\n"          // argc -> x0
    "add x1, sp, 8\n"         // argv -> x1
    "add x2, x0, 1\n"
    "add x2, x1, x2, lsl 3\n" // envp -> x2
    "and sp, x1, -16\n"       // Make sure stack is 16 byte aligned.
    "bl " hc_XSTR(_start_FUNC) "\n"
    "mov x8, 94\n"
    "svc 0\n"                 // Run exit_group with the return from start.
);
#elif hc_RISCV64
asm(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    ".option push\n"
    ".option norelax\n"           // `norelax` option has to be set for the following instruction.
    "lla gp, __global_pointer$\n" // Set gp to `__global_pointer$` which is provided by linker.
    ".option pop\n"               // Reset options.
    "ld a0, 0(sp)\n"              // argc -> a0
    "addi a1, sp, 8\n"            // argv -> a1
    "addi a2, a0, 1\n"
    "slli a2, a2, 3\n"
    "add a2, a2, a1\n"            // envp -> a2
    "andi sp, a1, -16\n"          // Make sure stack is 16 byte aligned.
    "call " hc_XSTR(_start_FUNC) "\n"
    "li a7, 94\n"
    "ecall\n"                     // Run exit_group with the return from start.
);
#endif

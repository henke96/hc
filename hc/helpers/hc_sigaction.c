#if hc_X86_64
void hc_sigaction_restore(void);
asm(
    ".section .text\n"
    ".local hc_sigaction_restore\n"
    "hc_sigaction_restore:\n"
	"mov $15, %rax\n"
	"syscall\n"
);
#endif

// Architecture independent version of hc_rt_sigaction().
// Just pretend the `sa_restorer` field of `struct sigaction` doesn't exist when using this.
static hc_ALWAYS_INLINE int32_t hc_sigaction(int32_t sig, struct sigaction *act, struct sigaction *oldact) {
#if hc_X86_64
    act->sa_flags |= SA_RESTORER;
    act->sa_restorer = hc_sigaction_restore;
#endif
    return hc_rt_sigaction(sig, act, oldact);
}
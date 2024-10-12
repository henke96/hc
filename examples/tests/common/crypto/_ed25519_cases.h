// Based on sign.input from http://ed25519.cr.yp.to/software.html.
// Entry N (0-1023) has format secret|expectedSignature|message, with sizes 32|64|N bytes.
extern const uint8_t _ed25519_cases[];
extern int32_t _ed25519_cases_size;

asm(
    ".section .rodata\n"
    ".global _ed25519_cases\n"
    ".align 8\n"
    "_ed25519_cases:\n"
    ".incbin \"_ed25519_cases\"\n"
    "_ed25519_cases_end:\n"
    ".global _ed25519_cases_size\n"
    ".align 4\n"
    "_ed25519_cases_size:\n"
    ".int _ed25519_cases_end - _ed25519_cases\n"
);

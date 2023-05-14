#include "hc/hc.h"
hc_ELF_EXPORT int32_t errno;

hc_ELF_EXPORT void __libc_start1(void) {}
hc_ELF_EXPORT void printf(void) {}
hc_ELF_EXPORT void abort(void) {}
hc_ELF_EXPORT void mmap(void) {}
hc_ELF_EXPORT void munmap(void) {}
hc_ELF_EXPORT void mprotect(void) {}
hc_ELF_EXPORT void openat(void) {}
hc_ELF_EXPORT void fstatat(void) {}
hc_ELF_EXPORT void read(void) {}
hc_ELF_EXPORT void write(void) {}
hc_ELF_EXPORT void close(void) {}
hc_ELF_EXPORT void chdir(void) {}

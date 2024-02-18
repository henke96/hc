#include "hc/hc.h"
hc_EXPORT int32_t errno;

extern char **environ;
extern char *__progname;
hc_EXPORT void __libc_start1(void) {
    environ = NULL;
    __progname = NULL;
}
hc_EXPORT void printf(void) {}
hc_EXPORT void abort(void) {}
hc_EXPORT void mmap(void) {}
hc_EXPORT void munmap(void) {}
hc_EXPORT void mprotect(void) {}
hc_EXPORT void openat(void) {}
hc_EXPORT void fstatat(void) {}
hc_EXPORT void read(void) {}
hc_EXPORT void write(void) {}
hc_EXPORT void close(void) {}
hc_EXPORT void chdir(void) {}
hc_EXPORT void elf_aux_info(void) {}
hc_EXPORT void getdents(void) {}

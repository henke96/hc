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
hc_EXPORT void clock_gettime(void) {}
hc_EXPORT void ppoll(void) {}
hc_EXPORT void memfd_create(void) {}
hc_EXPORT void ftruncate(void) {}
hc_EXPORT void socket(void) {}
hc_EXPORT void connect(void) {}
hc_EXPORT void sendmsg(void) {}
hc_EXPORT void recvfrom(void) {}
hc_EXPORT void writev(void) {}
hc_EXPORT void sendto(void) {}
hc_EXPORT void dlopen(void) {}
hc_EXPORT void dlerror(void) {}
hc_EXPORT void dlsym(void) {}
hc_EXPORT void dlclose(void) {}
hc_EXPORT void ioctl(void) {}
hc_EXPORT void mkdirat(void) {}

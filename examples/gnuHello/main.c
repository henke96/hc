#include "../../src/hc.h"
#include "../../src/util.c"
#include "../../src/libc/small.c"
#include "../../src/linux/linux.h"
#include "../../src/linux/sys.c"
#include "../../src/linux/helpers/_start.c"
#include "../../src/linux/gnu/libdl.so.2.h"

int32_t main(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    void *handle = dlopen("libc.so.6", RTLD_NOW);
    char *error = dlerror();
    if (error != NULL) {
        sys_writev(STDOUT_FILENO, (struct iovec[2]) {
            { .iov_base = &error[0],    .iov_len = util_cstrLen(error) },
            { .iov_base = (char *)"\n", .iov_len = 1 }
        }, 2);
        return 1;
    }
    int32_t (*printf)(const char *restrict format, ...) = dlsym(handle, "printf");
    error = dlerror();
    if (error != NULL) {
        sys_writev(STDOUT_FILENO, (struct iovec[2]) {
            { .iov_base = &error[0],    .iov_len = util_cstrLen(error) },
            { .iov_base = (char *)"\n", .iov_len = 1 }
        }, 2);
        return 1;
    }
    printf("Hello world!\n");
    return 0;
}
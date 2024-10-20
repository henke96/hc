#include "hc/hc.h"
#include "hc/wasm/wasi/wasi.h"

void noreturn _start(void) {
    struct ciovec iov = { hc_STR_COMMA_LEN("Hello!\n") };
    ssize_t written;
    uint16_t ret = fd_write(2, &iov, 1, &written);
    proc_exit(!!ret);
}

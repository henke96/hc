#include "hc/hc.h"
#include "hc/wasm/wasi.h"

void noreturn _start(void) {
    struct ciovec iov = {
        .buffer = "Hello!\n",
        .bufferLength = 7
    };
    uint32_t ret;
    fd_write(1, &iov, 1, &ret);
    proc_exit(ret);
}

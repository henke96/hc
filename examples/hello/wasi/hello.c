#include "hc/hc.h"
#include "hc/wasm/wasi.h"

void noreturn _start(void) {
    struct ciovec iov = { hc_STR_COMMA_LEN("Hello!\n") };
    uint32_t ret = fd_write(1, &iov, 1, &ret);
    proc_exit(!!ret);
}

static_assert(hc_ILP32, "Pointers not 32 bit");

#define CLOCK_MONOTONIC 1

struct ciovec {
    const char *buffer;
    ssize_t bufferLength;
};

hc_WASM_IMPORT("wasi_snapshot_preview1", "fd_write")
uint16_t fd_write(int32_t fd, const struct ciovec *iovs, ssize_t iovsLen, ssize_t *ret);

hc_WASM_IMPORT("wasi_snapshot_preview1", "proc_exit")
void noreturn proc_exit(uint32_t exitCode);

hc_WASM_IMPORT("wasi_snapshot_preview1", "clock_time_get")
uint16_t clock_time_get(uint32_t clockid, int64_t precision, int64_t *timestamp);

typedef int32_t util_STREAM_T;
#define util_STDIN 0
#define util_STDOUT 1
#define util_STDERR 2

static noreturn void util_abort(void) {
    hc_TRAP;
}

hc_WASM_IMPORT("env", "util_writeAll")
int32_t util_writeAll(int32_t fd, const void *buffer, ssize_t size);

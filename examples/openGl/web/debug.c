hc_WASM_IMPORT("env", "debug_printNum")
void _debug_printNum(const char *pre, int64_t num, const char *post);

static void debug_printNum(const char *pre, int64_t num, const char *post) {
    return _debug_printNum(pre, num, post);
}

hc_WASM_IMPORT("env", "debug_printNum")
void _debug_printNum(const char *pre, int64_t num, const char *post);

static void debug_printNum(const char *pre, int64_t num, const char *post) {
    return _debug_printNum(pre, num, post);
}

hc_WASM_IMPORT("env", "debug_fail")
noreturn void _debug_fail(int64_t res, const char *expression, const char *file, int32_t line);

static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    _debug_fail(res, expression, file, line);
}

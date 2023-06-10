hc_WASM_IMPORT("env", "debug_printNum")
void debug_printNum(const char *pre, int64_t num, const char *post);

hc_WASM_IMPORT("env", "debug_fail")
void debug_fail(int64_t res, const char *expression, const char *file, int32_t line);

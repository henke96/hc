static noreturn hc_INLINE void debug_abort(void) {
    abort();
}

hc_UNUSED
static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    printf("%s:%d fail: %s = %lld\n", file, line, expression, res);
    debug_abort();
}

static void hc_INLINE debug_print(const char *str) {
    printf(str);
}

static void hc_INLINE debug_printNum(const char *pre, int64_t num, const char *post) {
    printf("%s%lld%s", pre, num, post);
}

hc_UNUSED
static noreturn void debug_abort(void) {
    __android_log_assert(NULL, "hc", NULL);
    sys_exit_group(137);
}

hc_UNUSED
static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    __android_log_print(android_LOG_INFO, "hc", "%s:%d fail: %s = %lld\n", file, line, expression, res);
    debug_abort();
}

hc_UNUSED
static void debug_print(const char *str) {
    __android_log_write(android_LOG_INFO, "hc", str);
}

hc_UNUSED
static void debug_printNum(const char *pre, int64_t num, const char *post) {
    __android_log_print(android_LOG_INFO, "hc", "%s%lld%s", pre, num, post);
}

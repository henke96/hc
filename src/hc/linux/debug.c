hc_UNUSED
static noreturn void debug_abort(void) {
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
}

hc_UNUSED
static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    char resBuffer[util_INT64_MAX_CHARS + 1];
    resBuffer[util_INT64_MAX_CHARS] = '\n';
    char *resStr = util_intToStr(&resBuffer[util_INT64_MAX_CHARS], res);

    char lineBuffer[util_INT32_MAX_CHARS + 1];
    char *lineStr = util_intToStr(&lineBuffer[util_INT32_MAX_CHARS + 1], line);
    *--lineStr = ':';

    struct iovec_const print[] = {
        { file, util_cstrLen(file) },
        { lineStr, (int64_t)(&lineBuffer[hc_ARRAY_LEN(lineBuffer)] - lineStr) },
        { hc_STR_COMMA_LEN(" fail: ") },
        { expression, util_cstrLen(expression) },
        { hc_STR_COMMA_LEN(" = ") },
        { resStr, (int64_t)(&resBuffer[hc_ARRAY_LEN(resBuffer)] - resStr) }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
    debug_abort();
}

hc_UNUSED
static void debug_print(const char *str) {
    sys_write(STDOUT_FILENO, &str[0], util_cstrLen(str));
}

hc_UNUSED
static void debug_printNum(const char *pre, int64_t num, const char *post) {
    char buffer[util_INT64_MAX_CHARS];
    char *numStr = util_intToStr(&buffer[hc_ARRAY_LEN(buffer)], num);

    struct iovec_const print[] = {
        { pre, util_cstrLen(pre) },
        { numStr, (int64_t)(&buffer[hc_ARRAY_LEN(buffer)] - numStr) },
        { post, util_cstrLen(post) }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
}

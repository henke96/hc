hc_UNUSED
static noreturn void debug_abort(void) {
    hc_TRAP;
}

hc_UNUSED
static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    char resBuffer[util_INT64_MAX_CHARS + 1];
    resBuffer[util_INT64_MAX_CHARS] = '\n';
    char *resStr = util_intToStr(&resBuffer[util_INT64_MAX_CHARS], res);

    char lineBuffer[util_INT32_MAX_CHARS + 1];
    char *lineStr = util_intToStr(&lineBuffer[util_INT32_MAX_CHARS + 1], line);
    *--lineStr = ':';

    struct ciovec print[] = {
        { file, util_cstrLen(file) },
        { lineStr, (ssize_t)(&lineBuffer[hc_ARRAY_LEN(lineBuffer)] - lineStr) },
        { hc_STR_COMMA_LEN(" fail: ") },
        { expression, util_cstrLen(expression) },
        { hc_STR_COMMA_LEN(" = ") },
        { resStr, (ssize_t)(&resBuffer[hc_ARRAY_LEN(resBuffer)] - resStr) }
    };
    ssize_t written;
    fd_write(1, &print[0], hc_ARRAY_LEN(print), &written);
    debug_abort();
}

hc_UNUSED
static void debug_print(const char *str) {
    struct ciovec iov = { str, util_cstrLen(str) };
    ssize_t written;
    fd_write(1, &iov, 1, &written);
}

hc_UNUSED
static void debug_printNum(const char *pre, int64_t num, const char *post) {
    char buffer[util_INT64_MAX_CHARS];
    char *numStr = util_intToStr(&buffer[hc_ARRAY_LEN(buffer)], num);

    struct ciovec print[] = {
        { pre, util_cstrLen(pre) },
        { numStr, (ssize_t)(&buffer[hc_ARRAY_LEN(buffer)] - numStr) },
        { post, util_cstrLen(post) }
    };
    ssize_t written;
    fd_write(1, &print[0], hc_ARRAY_LEN(print), &written);
}

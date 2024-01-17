hc_UNUSED
static noreturn void debug_abort(void) {
    TerminateProcess(GetCurrentProcess(), 137);
    hc_UNREACHABLE;
}

hc_UNUSED
static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    char resBuffer[util_INT64_MAX_CHARS + 1];
    resBuffer[util_INT64_MAX_CHARS] = '\n';
    char *resStr = util_intToStr(&resBuffer[util_INT64_MAX_CHARS], res);

    char lineBuffer[util_INT32_MAX_CHARS + 1];
    char *lineStr = util_intToStr(&lineBuffer[util_INT32_MAX_CHARS + 1], line);
    *--lineStr = ':';

    void *stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    util_writeAll(stdoutHandle, file, util_cstrLen(file));
    util_writeAll(stdoutHandle, lineStr, &lineBuffer[hc_ARRAY_LEN(lineBuffer)] - lineStr);
    util_writeAll(stdoutHandle, hc_STR_COMMA_LEN(" fail: "));
    util_writeAll(stdoutHandle, expression, util_cstrLen(expression));
    util_writeAll(stdoutHandle, hc_STR_COMMA_LEN(" = "));
    util_writeAll(stdoutHandle, resStr, &resBuffer[hc_ARRAY_LEN(resBuffer)] - resStr);
    debug_abort();
}

hc_UNUSED
static void debug_print(const char *str) {
    void *stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    util_writeAll(stdoutHandle, str, util_cstrLen(str));
}

hc_UNUSED
static void debug_printNum(const char *pre, int64_t num, const char *post) {
    char buffer[util_INT64_MAX_CHARS];
    char *numStr = util_intToStr(&buffer[util_INT64_MAX_CHARS], num);

    void *stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    util_writeAll(stdoutHandle, pre, util_cstrLen(pre));
    util_writeAll(stdoutHandle, numStr, &buffer[hc_ARRAY_LEN(buffer)] - numStr);
    util_writeAll(stdoutHandle, post, util_cstrLen(post));
}

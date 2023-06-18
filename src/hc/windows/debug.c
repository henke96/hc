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

    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(stdOutHandle, file, (uint32_t)util_cstrLen(file), NULL, NULL);
    WriteFile(stdOutHandle, lineStr, (uint32_t)(&lineBuffer[hc_ARRAY_LEN(lineBuffer)] - lineStr), NULL, NULL);
    WriteFile(stdOutHandle, hc_STR_COMMA_LEN(" fail: "), NULL, NULL);
    WriteFile(stdOutHandle, expression, (uint32_t)util_cstrLen(expression), NULL, NULL);
    WriteFile(stdOutHandle, hc_STR_COMMA_LEN(" = "), NULL, NULL);
    WriteFile(stdOutHandle, resStr, (uint32_t)(&resBuffer[hc_ARRAY_LEN(resBuffer)] - resStr), NULL, NULL);
    debug_abort();
}

hc_UNUSED
static void debug_print(const char *str) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(stdOutHandle, str, (uint32_t)util_cstrLen(str), NULL, NULL);
}

hc_UNUSED
static void debug_printNum(const char *pre, int64_t num, const char *post) {
    char buffer[util_INT64_MAX_CHARS];
    char *numStr = util_intToStr(&buffer[util_INT64_MAX_CHARS], num);

    int64_t preLen = util_cstrLen(pre);
    debug_ASSERT(preLen <= UINT32_MAX);
    int64_t postLen = util_cstrLen(post);
    debug_ASSERT(postLen <= UINT32_MAX);

    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(stdOutHandle, pre, (uint32_t)preLen, NULL, NULL);
    WriteFile(stdOutHandle, numStr, (uint32_t)(&buffer[hc_ARRAY_LEN(buffer)] - numStr), NULL, NULL);
    WriteFile(stdOutHandle, post, (uint32_t)postLen, NULL, NULL);
}

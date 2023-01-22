hc_UNUSED
static noreturn hc_COLD void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
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
    ExitProcess(137);
}

#ifdef debug_NDEBUG
    #define debug_ASSERT(EXPR)
    #define debug_ASSUME(EXPR) hc_ASSUME(EXPR)
    #define debug_CHECK(EXPR, COND) EXPR
#else
    #define debug_ASSERT(EXPR) ((void)((EXPR) || (debug_fail(0, #EXPR, __FILE_NAME__, __LINE__), 0)))
    #define debug_ASSUME debug_ASSERT
    #define debug_CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)
#endif

hc_UNUSED
static hc_COLD void debug_printNum(const char *pre, int64_t num, const char *post) {
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

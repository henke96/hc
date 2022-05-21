// TODO: We are just assuming the uint32_t casts are fine.

hc_UNUSED
static void debug_printNum(const char *pre, int64_t num, const char *post) {
    char buffer[util_INT64_MAX_CHARS];
    char *numStr = util_intToStr(&buffer[util_INT64_MAX_CHARS], num);

    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(stdOutHandle, pre, (uint32_t)util_cstrLen(pre), NULL, NULL);
    WriteFile(stdOutHandle, numStr, (uint32_t)(&buffer[util_INT64_MAX_CHARS] - numStr), NULL, NULL);
    WriteFile(stdOutHandle, post, (uint32_t)util_cstrLen(post), NULL, NULL);
}

hc_UNUSED
static void debug_printStr(const char *pre, const char *str, const char *post, int64_t strlen) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(stdOutHandle, pre, (uint32_t)util_cstrLen(pre), NULL, NULL);
    WriteFile(stdOutHandle, str, (uint32_t)strlen, NULL, NULL);
    WriteFile(stdOutHandle, post, (uint32_t)util_cstrLen(post), NULL, NULL);
}

#ifdef debug_NDEBUG
    #define debug_ASSERT(EXPR) ((void)0)
    #define debug_CHECK(EXPR, COND) EXPR
#else
    #define debug_ASSERT(EXPR) ((void)((EXPR) || (debug_failAssert(#EXPR, __FILE__, __func__, __LINE__), 0)))
    #define debug_CHECK(EXPR, COND) ((void)((EXPR COND) || (debug_failAssert(#EXPR " " #COND, __FILE__, __func__, __LINE__), 0)))
hc_UNUSED
static noreturn void debug_failAssert(const char *expression, const char *file, const char *function, int32_t line) {
    char buffer[util_INT32_MAX_CHARS];
    char *lineStr = util_intToStr(&buffer[util_INT32_MAX_CHARS], line);

    static const char start[18] = "Assertion failed: ";
    static const char parenStart[2] = " (";
    static const char colon[2] = ": ";
    static const char end[2] = ")\n";

    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(stdOutHandle, &start[0], sizeof(start), NULL, NULL);
    WriteFile(stdOutHandle, expression, (uint32_t)util_cstrLen(expression), NULL, NULL);
    WriteFile(stdOutHandle, &parenStart[0], sizeof(parenStart), NULL, NULL);
    WriteFile(stdOutHandle, file, (uint32_t)util_cstrLen(file), NULL, NULL);
    WriteFile(stdOutHandle, &colon[0], 1, NULL, NULL);
    WriteFile(stdOutHandle, lineStr, (uint32_t)(&buffer[util_INT32_MAX_CHARS] - lineStr), NULL, NULL);
    WriteFile(stdOutHandle, &colon[0], sizeof(colon), NULL, NULL);
    WriteFile(stdOutHandle, function, (uint32_t)util_cstrLen(function), NULL, NULL);
    WriteFile(stdOutHandle, &end[0], sizeof(end), NULL, NULL);
    ExitProcess(137);
}
#endif

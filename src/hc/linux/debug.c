hc_UNUSED
static void hc_COLD debug_printNum(const char *pre, int64_t num, const char *post) {
    char buffer[util_INT64_MAX_CHARS];
    char *numStr = util_intToStr(&buffer[hc_ARRAY_LEN(buffer)], num);

    struct iovec print[] = {
        { (char *)pre, util_cstrLen(pre) },
        { numStr, (int64_t)(&buffer[hc_ARRAY_LEN(buffer)] - numStr) },
        { (char *)post, util_cstrLen(post) }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
}

hc_UNUSED
static noreturn hc_COLD void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    char resBuffer[util_INT64_MAX_CHARS + 1];
    resBuffer[util_INT64_MAX_CHARS] = '\n';
    char *resStr = util_intToStr(&resBuffer[util_INT64_MAX_CHARS], res);

    char lineBuffer[util_INT32_MAX_CHARS + 1];
    char *lineStr = util_intToStr(&lineBuffer[util_INT32_MAX_CHARS + 1], line);
    *--lineStr = ':';

    struct iovec print[] = {
        { (char *)file, util_cstrLen(file) },
        { lineStr, (int64_t)(&lineBuffer[hc_ARRAY_LEN(lineBuffer)] - lineStr) },
        { hc_STR_COMMA_LEN(" fail: ") },
        { (char *)expression, util_cstrLen(expression) },
        { hc_STR_COMMA_LEN(" = ") },
        { resStr, (int64_t)(&resBuffer[hc_ARRAY_LEN(resBuffer)] - resStr) }
    };
    sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
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

#if hc_DEBUG
    #define debug_ASSERT(EXPR) ((void)((EXPR) || (debug_fail(0, #EXPR, __FILE_NAME__, __LINE__), 0)))
    #define debug_ASSUME debug_ASSERT
    #define debug_CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)
#else
    #define debug_ASSERT(EXPR)
    #define debug_ASSUME(EXPR) hc_ASSUME(EXPR)
    #define debug_CHECK(EXPR, COND) EXPR
#endif

static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line) {
    util_STREAM_T stderr = util_STDERR;
    util_writeAll(stderr, file, util_cstrLen(file));
    {
        char print[hc_STR_LEN(":") + util_INT32_MAX_CHARS + hc_STR_LEN(" fail: ")];
        char *pos = hc_ARRAY_END(print);
        hc_PREPEND_STR(pos, " fail: ");
        pos = util_intToStr(pos, line);
        hc_PREPEND_STR(pos, ":");
        util_writeAll(stderr, pos, hc_ARRAY_END(print) - pos);
    }
    util_writeAll(stderr, expression, util_cstrLen(expression));
    {
        char print[hc_STR_LEN(" is ") + util_INT64_MAX_CHARS + hc_STR_LEN("\n")];
        char *pos = hc_ARRAY_END(print);
        hc_PREPEND_STR(pos, "\n");
        pos = util_intToStr(pos, res);
        hc_PREPEND_STR(pos, " is ");
        util_writeAll(stderr, pos, hc_ARRAY_END(print) - pos);
    }
    util_abort();
}

static void debug_print(char *str) {
    debug_CHECK(util_writeAll(util_STDERR, str, util_cstrLen(str)), RES == 0);
}

static void debug_printNum(char *str, int64_t num) {
    util_STREAM_T stderr = util_STDERR;
    debug_CHECK(util_writeAll(stderr, str, util_cstrLen(str)), RES == 0);
    char print[hc_STR_LEN(" (") + util_INT64_MAX_CHARS + hc_STR_LEN(")\n")];
    char *pos = hc_ARRAY_END(print);
    hc_PREPEND_STR(pos, ")\n");
    pos = util_intToStr(pos, num);
    hc_PREPEND_STR(pos, " (");
    debug_CHECK(util_writeAll(stderr, pos, hc_ARRAY_END(print) - pos), RES == 0);
}

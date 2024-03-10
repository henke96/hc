hc_UNUSED static noreturn void debug_abort(void);
hc_UNUSED static noreturn void debug_fail(int64_t res, const char *expression, const char *file, int32_t line);
hc_UNUSED static void debug_print(const char *str);
hc_UNUSED static void debug_printNum(const char *pre, int64_t num, const char *post);

#if hc_DEBUG
    #define debug_ASSERT(EXPR) ((void)((EXPR) || (debug_fail(0, #EXPR, __FILE_NAME__, __LINE__), 0)))
    #define debug_ASSUME debug_ASSERT
    #define debug_CHECK(EXPR, COND) do { typeof(EXPR) RES = (EXPR); if (!(COND)) debug_fail((int64_t)RES, #EXPR, __FILE_NAME__, __LINE__); } while (0)
#else
    #define debug_ASSERT(EXPR)
    #define debug_ASSUME(EXPR) hc_ASSUME(EXPR)
    #define debug_CHECK(EXPR, COND) EXPR
#endif

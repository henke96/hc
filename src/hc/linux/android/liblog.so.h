enum android_logPriority {
    android_LOG_UNKNOWN = 0,
    android_LOG_DEFAULT,
    android_LOG_VERBOSE,
    android_LOG_DEBUG,
    android_LOG_INFO,
    android_LOG_WARN,
    android_LOG_ERROR,
    android_LOG_FATAL,
    android_LOG_SILENT
};

int32_t __android_log_write(int32_t prio, const char *tag, const char *text);
int32_t __android_log_print(int32_t prio, const char* tag, const char *fmt, ...) __attribute__((__format__(printf, 3, 4)));
void noreturn __android_log_assert(const char *cond, const char *tag, const char *fmt, ...) __attribute__((__format__(printf, 3, 4)));

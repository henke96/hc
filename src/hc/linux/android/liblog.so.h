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

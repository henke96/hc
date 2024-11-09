#define argParse_START(ARGV, ARGV_INDEX, CUR_OPT, DONE_LABEL) \
    for (CUR_OPT = '\0';; ++ARGV_INDEX) { \
        if (CUR_OPT == '-') { \
            CUR_OPT = '\0'; \
            goto DONE_LABEL; \
        } \
        char *ARG = ARGV[ARGV_INDEX]; \
        if (ARG == NULL) { \
            if (CUR_OPT != '\0') --ARGV_INDEX; \
            goto DONE_LABEL; \
        } \
        switch (CUR_OPT) { \
            default: --ARGV_INDEX; goto DONE_LABEL; \
            case '\0': { \
                if (*ARG != '-') goto DONE_LABEL; \
                while (*++ARG != '\0') { \
                    switch (*ARG) { \
                        default: { \
                            CUR_OPT = *ARG; \
                            if (ARG[1] != '\0') goto DONE_LABEL; \
                            break; \
                        }

#define argParse_FLAGS_TO_ARGS_DELIMITER \
                    } \
                } \
                continue; \
            }

#define argParse_END(CUR_OPT) \
        } \
        CUR_OPT = '\0'; \
    } \

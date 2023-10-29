struct options {
    char *multicastGroups;
    char *interface;
    int32_t interfaceSize;
    uint16_t port;
    uint8_t bindAddress[4];
    char __pad[6];
};

static int32_t options_init(struct options *self, int32_t argc, char **argv) {
    *self = (struct options) {
        .multicastGroups = NULL,
        .interface = NULL,
        .interfaceSize = 0,
        .port = 5001,
        .bindAddress = { 0, 0, 0, 0 },
    };

    char prevOpt = '\0';
    while (--argc > 0) {
        char *arg = *++argv;
        switch (prevOpt) {
            case 'm': {
                self->multicastGroups = arg;
                break;
            }
            case 'b': {
                for (int32_t i = 0;;) {
                    uint64_t octet;
                    int32_t parsed = util_strToUint(arg, INT32_MAX, &octet);
                    if (parsed <= 0 || octet > 255) goto optsDone;
                    self->bindAddress[i] = (uint8_t)octet;

                    if (++i == hc_ARRAY_LEN(self->bindAddress)) {
                        if (arg[parsed] != '\0') goto optsDone;
                        break;
                    } else if (arg[parsed] != '.') goto optsDone;
                    arg += parsed + 1;
                }
                break;
            }
            case 'p': {
                uint64_t port;
                int32_t parsed = util_strToUint(arg, INT32_MAX, &port);
                if (parsed <= 0 || port > UINT16_MAX || arg[parsed] != '\0') goto optsDone;
                self->port = (uint16_t)port;
                break;
            }
            case 'i': {
                int64_t ifLen = util_cstrLen(arg);
                if (ifLen > IFNAMSIZ - 1) goto optsDone;
                self->interface = arg;
                self->interfaceSize = (int32_t)ifLen + 1;
                break;
            }
            case '-': {
                prevOpt = '\0';
                goto optsDone;
            }
            case '\0': {
                if (arg[0] != '-') goto optsDone;
                while (*++arg != '\0') {
                    switch (*arg) {
                        default: {
                            prevOpt = *arg;
                            if (arg[1] != '\0') goto optsDone;
                        }
                    }
                }
                continue;
            }
            default: goto optsDone;
        }
        prevOpt = '\0';
    }
    optsDone:;
    if (prevOpt != '\0') {
        struct iovec_const print[] = {
            { hc_STR_COMMA_LEN("Invalid option `") },
            { &prevOpt, 1 },
            { hc_STR_COMMA_LEN("`\n") },
        };
        sys_writev(STDOUT_FILENO, &print[0], hc_ARRAY_LEN(print));
        return -1;
    }
    if (argc != 0) {
        sys_write(STDOUT_FILENO, hc_STR_COMMA_LEN("Too many arguments\n"));
        return -1;
    }
    return 0;
}

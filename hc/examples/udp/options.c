struct options {
    char *multicastGroups;
    char *interface;
    int32_t interfaceSize;
    uint16_t port;
    uint8_t bindAddress[4];
    char __pad[6];
};

static int32_t options_init(struct options *self, char **argv) {
    *self = (struct options) {
        .multicastGroups = NULL,
        .interface = NULL,
        .interfaceSize = 0,
        .port = 5001,
        .bindAddress = { 0, 0, 0, 0 },
    };

    char curOpt;
    int32_t argvIndex = 1;
    argParse_START(argv, argvIndex, curOpt, optsDone)
    argParse_FLAGS_TO_ARGS_DELIMITER
        case 'm': {
            self->multicastGroups = ARG;
            break;
        }
        case 'b': {
            for (int32_t i = 0;;) {
                uint64_t octet;
                int32_t parsed = util_strToUint(ARG, INT32_MAX, &octet);
                if (parsed <= 0 || octet > 255) goto optsDone;
                self->bindAddress[i] = (uint8_t)octet;

                if (++i == hc_ARRAY_LEN(self->bindAddress)) {
                    if (ARG[parsed] != '\0') goto optsDone;
                    break;
                } else if (ARG[parsed] != '.') goto optsDone;
                ARG += parsed + 1;
            }
            break;
        }
        case 'p': {
            uint64_t port;
            int32_t parsed = util_strToUint(ARG, INT32_MAX, &port);
            if (parsed <= 0 || port > UINT16_MAX || ARG[parsed] != '\0') goto optsDone;
            self->port = (uint16_t)port;
            break;
        }
        case 'i': {
            int64_t ifLen = util_cstrLen(ARG);
            if (ifLen > IFNAMSIZ - 1) goto optsDone;
            self->interface = ARG;
            self->interfaceSize = (int32_t)ifLen + 1;
            break;
        }
    argParse_END(curOpt)

    optsDone:
    if (curOpt != '\0') {
        #define _PRINT1 "Invalid option `"
        #define _PRINT2 "` at position "
        char print[
            hc_STR_LEN(_PRINT1) +
            hc_STR_LEN("x") +
            hc_STR_LEN(_PRINT2) +
            util_INT32_MAX_CHARS +
            hc_STR_LEN("\n")
        ];
        char *pos = &print[sizeof(print)];
        *--pos = '\n';
        pos = util_intToStr(pos, argvIndex);
        pos = hc_MEMCPY(pos - hc_STR_LEN(_PRINT2), hc_STR_COMMA_LEN(_PRINT2));
        *--pos = curOpt;
        pos = hc_MEMCPY(pos - hc_STR_LEN(_PRINT1), hc_STR_COMMA_LEN(_PRINT1));
        sys_write(2, pos, &print[sizeof(print)] - pos);
        return -1;
    }
    if (argv[argvIndex] != NULL) {
        sys_write(2, hc_STR_COMMA_LEN("Too many arguments\n"));
        return -1;
    }
    return 0;
}

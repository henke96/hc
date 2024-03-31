static int32_t init(char **envp);
static void deinit(void);
static int32_t readIntoBuffer(void);
static int32_t writeBuffer(int32_t size);
static int32_t openOutput(char *name);
static void closeOutput(void);
static int32_t add(char *name, char *root);

#define NAME 0
#define MODE 100
#define SIZE 124
#define MTIME 136
#define CHKSUM 148
#define TYPEFLAG 156
#define MAGIC 257
#define VERSION 263
#define PREFIX 345
#define END 512

#define MAX_NAME_LEN 100
#define MAX_PREFIX_LEN 155

static char octalTable[8] = "01234567";
static char buffer[65536] hc_ALIGNED(16);
static char prefix[MAX_PREFIX_LEN + hc_STR_LEN("/") + MAX_NAME_LEN];
static int32_t prefixLen = 0;

static bool isDot(char *name) {
    return name[0] == '.' && name[1] == '\0';
}

static bool isDotDot(char *name) {
    return name[0] == '.' && name[1] == '.' && name[2] == '\0';
}

static void leaveDirectory(char *name) {
    if (isDot(name)) return;
    while (prefixLen > 0) {
        --prefixLen;
        if (prefix[prefixLen] == '/') break;
    }
}

static int32_t writeRecord(char *name, int32_t nameLen, int64_t fileSize) {
    if (isDot(name)) return 0;
    if (
        nameLen > MAX_NAME_LEN ||
        prefixLen > MAX_PREFIX_LEN ||
        fileSize > 077777777777
    ) return -1;

    // Fill in name and prefix, zero rest.
    hc_MEMCPY(&buffer[NAME], name, (size_t)nameLen);
    hc_MEMSET(&buffer[NAME + nameLen], 0, (size_t)(PREFIX - (NAME + nameLen)));
    hc_MEMCPY(&buffer[PREFIX], prefix, (size_t)prefixLen);
    hc_MEMSET(&buffer[PREFIX + prefixLen], 0, (size_t)(END - (PREFIX + prefixLen)));

    char typeflag = '0';
    if (fileSize < 0) { // A directory.
        typeflag = '5';
        fileSize = 0;

        // Update prefix since we will enter this directory.
        if (prefixLen > 0) prefix[prefixLen++] = '/';
        hc_MEMCPY(&prefix[prefixLen], name, (uint64_t)nameLen);
        prefixLen += nameLen;
    }

    // Fill in other fields.
    hc_MEMCPY(&buffer[MODE], hc_STR_COMMA_LEN("0000777"));
    for (int32_t i = 0; i < 11; ++i) {
        buffer[SIZE + 10 - i] = octalTable[(fileSize >> 3 * i) & 7];
    }
    hc_MEMCPY(&buffer[MTIME], hc_STR_COMMA_LEN("00000000000"));
    hc_MEMCPY(&buffer[CHKSUM], hc_STR_COMMA_LEN("        "));
    buffer[TYPEFLAG] = typeflag;
    hc_MEMCPY(&buffer[MAGIC], hc_STR_COMMA_LEN("ustar"));
    hc_MEMCPY(&buffer[VERSION], hc_STR_COMMA_LEN("00"));

    // Calculate checksum.
    int32_t checksum = 0;
    for (int32_t i = 0; i < END; ++i) {
        checksum += buffer[i];
    }
    for (int32_t i = 0; i < 7; ++i) {
        buffer[CHKSUM + 6 - i] = octalTable[(checksum >> 3 * i) & 7];
    }

    // Write header.
    if (writeBuffer(END) < 0) return -2;
    if (fileSize == 0) return 0;

    // Write file content.
    int64_t totalWritten = 0;
    for (;;) {
        int32_t numRead = readIntoBuffer();
        if (numRead <= 0) {
            if (numRead < 0) return -3;
            if (totalWritten != fileSize) return -4;
            break;
        }
        if (writeBuffer(numRead) < 0) return -5;
        totalWritten += numRead;
    }
    int32_t numPadBytes = math_PAD_BYTES(totalWritten, 512);
    hc_MEMSET(&buffer[0], 0, (uint64_t)numPadBytes);
    if (writeBuffer(numPadBytes) < 0) return -6;
    return 0;
}

static void sortNames(char **names, int64_t length) {
    for (int64_t i = 1; i < length; ++i) {
        int64_t j = i;
        do {
            char *prevName = names[j - 1];
            if (util_cstrCmp(prevName, names[j]) <= 0) break;
            names[j - 1] = names[j];
            names[j] = prevName;
        } while (--j > 0);
    }
}

// Returns length if valid, else negative error.
static int32_t validateName(char *name, int32_t maxLen) {
    if (isDotDot(name)) return -1;
    for (int32_t i = 0;; ++i) {
        if (name[i] == '\0') return i;
        if (i >= maxLen) return -2;
        if (name[i] == '/') return -3;
    }
}

static int32_t finaliseOutput(void) {
    hc_MEMSET(&buffer[0], 0, 2 * END);
    int32_t status = writeBuffer(2 * END);
    closeOutput();
    return status;
}

static int32_t run(int32_t argc, char **argv) {
    bool outputOpen = false;
    char *root = ".";

    char prevOpt = '\0';
    int32_t argI = 0;
    while (--argc > 0) {
        char *arg = argv[++argI];
        switch (prevOpt) {
            case 'o': {
                if (outputOpen) finaliseOutput();
                int32_t status = openOutput(arg);
                if (status < 0) {
                    debug_printNum("Failed to open output (", status, ")\n");
                    return -1;
                }
                outputOpen = true;
                break;
            }
            case 'a': {
                if (!outputOpen) {
                    debug_print("No output opened\n");
                    return -1;
                }
                if (validateName(arg, MAX_NAME_LEN) < 0) {
                    debug_print("Invalid name\n");
                    return -1;
                }
                int32_t status = add(arg, root);
                if (status < 0) {
                    debug_printNum("Failed to add entry (", status, ")\n");
                    return -1;
                }
                break;
            }
            case 'p': {
                if (!outputOpen) {
                    debug_print("No output opened\n");
                    return -1;
                }
                int32_t len = validateName(arg, MAX_PREFIX_LEN);
                if (len < 0) {
                    debug_print("Invalid prefix\n");
                    return -1;
                }
                prefixLen = 0;
                if (len > 0 && writeRecord(arg, len, -1) < 0) {
                    debug_print("Failed to add prefix\n");
                    return -1;
                }
                break;
            }
            case 'r': {
                root = arg;
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
        debug_print("Invalid option\n");
        return -1;
    }
    if (argc != 0) {
        debug_print("Too many arguments\n");
        return -1;
    }
    if (outputOpen) return finaliseOutput();
    return 0;
}

int32_t start(int32_t argc, char **argv, char **envp) {
    if (init(envp) < 0) return 1;
    if (run(argc, argv) < 0) {
        debug_print("Usage: tar [-o OUTPUT_FILE | -r ROOT | -p PREFIX | -a FILE]...\n");
        return 1;
    }
    deinit();
    return 0;
}

static void initPageSize(char **envp);
static int32_t init(char **includePaths);
static void deinit(void);
static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceSize, char *path, int32_t pathLen, bool asBase64);
static int32_t writeToFile(char *path, char *content, int64_t contentSize);

static struct allocator alloc;
static char *buffer;
static int64_t bufferSize = 0;

static int64_t findPattern(int64_t bufferStartI, char *pattern, int64_t patternSize) {
    for (int64_t bufferI = bufferStartI; bufferI < bufferSize - patternSize; ++bufferI) {
        for (int64_t patternI = 0; patternI < patternSize; ++patternI) {
            if (buffer[bufferI + patternI] != pattern[patternI]) goto noMatch;
        }
        return bufferI;
        noMatch:;
    }
    return -1;
}

static int32_t handleInclude(char *startPattern, char *endPattern, bool asBase64) {
    int64_t startPatternLen = util_cstrLen(startPattern);
    int64_t startPatternI = findPattern(0, startPattern, startPatternLen);
    if (startPatternI < 0) return 1;
    int64_t endPatternLen = util_cstrLen(endPattern);
    int64_t endPatternI = findPattern(startPatternI, endPattern, endPatternLen);
    if (endPatternI < 0) {
        debug_print("Unclosed include!\n");
        return -1;
    }
    int64_t nameI = startPatternI + startPatternLen;
    int64_t nameLen = endPatternI - nameI;
    if (nameLen > INT32_MAX) return -1;

    int32_t status = replaceWithFile(
        startPatternI,
        endPatternI + endPatternLen - startPatternI,
        &buffer[nameI],
        (int32_t)nameLen,
        asBase64
    );
    if (status < 0) {
        debug_printNum("Failed to handle include (", status, ")\n");
        return -1;
    }
    return 0;
}

int32_t start(int32_t argc, char **argv, char **envp) {
    if (argc < 3) return 1;
    char *inputName = argv[2];
    int64_t inputNameLen = util_cstrLen(inputName);
    if (inputNameLen > INT32_MAX) return 1;

    initPageSize(envp);
    if (allocator_init(&alloc, (int64_t)1 << 32) < 0) return 1;

    int32_t status;
    status = init(&argv[3]);
    if (status < 0) {
        debug_printNum("Failed to initialise (", status, ")\n");
        status = 1;
        goto cleanup_alloc;
    }

    status = replaceWithFile(0, 0, inputName, (int32_t)inputNameLen, false);
    if (status < 0) {
        debug_printNum("Failed to read input (", status, ")\n");
        status = 1;
        goto cleanup_init;
    }

    int32_t complete = 0;
    while (!complete) {
        complete = 1;
        status = handleInclude("<!--INCLUDE(", ")-->", false);
        if (status < 0) {
            status = 1;
            goto cleanup_init;
        }
        complete &= status;
        status = handleInclude("/*INCLUDE(", ")*/", false);
        if (status < 0) {
            status = 1;
            goto cleanup_init;
        }
        complete &= status;
        status = handleInclude("/*INCLUDE_BASE64(", ")*/", true);
        if (status < 0) {
            status = 1;
            goto cleanup_init;
        }
        complete &= status;
    }

    status = writeToFile(argv[1], buffer, bufferSize);
    if (status != 0) {
        debug_printNum("Failed to write output (", status, ")\n");
        status = 1;
    }

    cleanup_init:
    deinit();
    cleanup_alloc:
    allocator_deinit(&alloc);
    return status;
}

static void initialise(char **envp);
static int32_t changeDir(char *path);
static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceLen, char *path, int32_t pathLen, bool asBase64);
static int32_t writeToFile(char *path, char *content, int64_t contentLen);

static struct allocator alloc;
static int64_t bufferLen = 0;

static int64_t findPattern(int64_t bufferStartI, char *pattern, int64_t patternLen) {
    for (int64_t bufferI = bufferStartI; bufferI < bufferLen - patternLen; ++bufferI) {
        for (int64_t patternI = 0; patternI < patternLen; ++patternI) {
            if (((char *)alloc.mem)[bufferI + patternI] != pattern[patternI]) goto noMatch;
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
        &alloc.mem[nameI],
        (int32_t)nameLen,
        asBase64
    );
    if (status < 0) {
        debug_printNum("Failed to handle include (", status, ")\n");
        return -1;
    }
    return 0;
}

#define common_ALLOC_RESERVE_SIZE ((int64_t)1 << 32)
int32_t start(int32_t argc, char **argv, char **envp) {
    if (argc != 4) return 1;
    initialise(envp);
    if (allocator_init(&alloc, common_ALLOC_RESERVE_SIZE) < 0) return 1;

    int32_t status = replaceWithFile(0, 0, argv[1], (int32_t)util_cstrLen(argv[1]), false);
    if (status < 0) return 1;

    status = changeDir(argv[2]);
    if (status < 0) return 1;

    int32_t complete = 0;
    while (!complete) {
        complete = 1;
        status = handleInclude("<!--INCLUDE(", ")-->", false);
        if (status < 0) {
            status = 1;
            goto cleanup_alloc;
        }
        complete &= status;
        status = handleInclude("/*INCLUDE(", ")*/", false);
        if (status < 0) {
            status = 1;
            goto cleanup_alloc;
        }
        complete &= status;
        status = handleInclude("/*INCLUDE_BASE64(", ")*/", true);
        if (status < 0) {
            status = 1;
            goto cleanup_alloc;
        }
        complete &= status;
    }

    int64_t outNameLen = util_cstrLen(argv[3]);
    int64_t outNameHtmlLen = outNameLen + (int64_t)sizeof(".html");
    char *outNameHtml = &alloc.mem[alloc.size];
    if (allocator_resize(&alloc, alloc.size + outNameHtmlLen) < 0) {
        status = 1;
        goto cleanup_alloc;
    }
    hc_MEMCPY(&outNameHtml[0], argv[3], (uint64_t)outNameLen);
    hc_MEMCPY(&outNameHtml[outNameLen], hc_STR_COMMA_LEN(".html\0"));
    status = writeToFile(outNameHtml, &alloc.mem[0], bufferLen);
    if (status < 0) {
        debug_printNum("Error: Failed to write html output (", status, ")\n");
        status = 1;
        goto cleanup_alloc;
    }

    status = 0;
    cleanup_alloc:
    allocator_deinit(&alloc, common_ALLOC_RESERVE_SIZE);
    return status;
}

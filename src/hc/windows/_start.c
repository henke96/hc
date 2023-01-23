int32_t start(int32_t argc, char **argv);

static int32_t _start_parseCmdLine(char *cmdLine) {
    int32_t argc = 1;
    char *dst = cmdLine;

    bool inQuote = false;
    for (char *cur = cmdLine; *cur != '\0'; ++cur) {
        if (*cur == ' ') {
            if (!inQuote) {
                ++argc;
                *dst++ = '\0';
                for (; cur[1] == ' '; ++cur);
                continue;
            }
        } else if (*cur == '"') {
            if (cur[1] == '"') {
                ++cur;
            } else {
                inQuote = !inQuote;
                continue;
            }
        }
        *dst++ = *cur;
    }
    *dst = '\0';
    return argc;
}

void noreturn _start(void) {
    int32_t status = 1;

    AttachConsole(ATTACH_PARENT_PROCESS);

    void *heap = GetProcessHeap();
    if (heap == NULL) goto cleanup_none;

    const uint16_t *cmdLineUtf16 = GetCommandLineW();
    int32_t cmdLineSize = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        cmdLineUtf16, -1,
        NULL, 0,
        NULL, NULL
    );
    if (cmdLineSize <= 0) goto cleanup_none;

    char *cmdLine = HeapAlloc(heap, 0, (uint64_t)cmdLineSize);
    if (cmdLine == NULL) goto cleanup_none;

    cmdLineSize = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        cmdLineUtf16, -1,
        cmdLine, cmdLineSize,
        NULL, NULL
    );
    if (cmdLineSize == 0) goto cleanup_cmdLine;

    int32_t argc = _start_parseCmdLine(cmdLine);
    char **argv = HeapAlloc(heap, 0, sizeof(char *) * (uint64_t)argc + 1);
    if (argv == NULL) goto cleanup_cmdLine;

    argv[0] = cmdLine;
    argv[argc] = NULL;

    char *cur = cmdLine;
    for (int32_t i = 1; i < argc; ++i) {
        for (; *cur != '\0'; ++cur);
        argv[i] = ++cur;
    }

    status = start(argc, argv);

    debug_CHECK(HeapFree(heap, 0, argv), RES != 0);
    cleanup_cmdLine:
    debug_CHECK(HeapFree(heap, 0, cmdLine), RES != 0);
    cleanup_none:
    ExitProcess((uint32_t)status);
}

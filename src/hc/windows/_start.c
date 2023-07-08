int32_t start(int32_t argc, char **argv, char **envp);

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
    if (cmdLineSize <= 0) goto cleanup_cmdLine;

    int32_t argc = _start_parseCmdLine(cmdLine);
    char **argv = HeapAlloc(heap, 0, sizeof(char *) * (uint64_t)(argc + 1));
    if (argv == NULL) goto cleanup_cmdLine;

    {
        char *curr = cmdLine;
        for (int32_t i = 0; i < argc; ++i) {
            argv[i] = curr;
            while (*curr++ != '\0');
        }
        argv[argc] = NULL;
    }

    uint16_t *envUtf16 = GetEnvironmentStringsW();
    if (envUtf16 == NULL) goto cleanup_argv;

    int32_t envUtf16Len = 2;
    int32_t envCount = 0;
    for (;; ++envUtf16Len) {
        if (envUtf16[envUtf16Len - 2] == u'\0') {
            ++envCount;
            if (envUtf16[envUtf16Len - 1] == u'\0') break;
        }
    }

    int32_t envSize = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        envUtf16, envUtf16Len,
        NULL, 0,
        NULL, NULL
    );
    if (envSize <= 0) goto cleanup_environment_strings;

    char *env = HeapAlloc(heap, 0, (uint64_t)envSize);
    if (env == NULL) goto cleanup_environment_strings;

    envSize = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        envUtf16, envUtf16Len,
        env, envSize,
        NULL, NULL
    );
    if (envSize <= 0) goto cleanup_env;

    char **envp = HeapAlloc(heap, 0, sizeof(char *) * (uint64_t)(envCount + 1));
    if (envp == NULL) goto cleanup_env;

    char *curr = env;
    for (int32_t i = 0; i < envCount; ++i) {
        envp[i] = curr;
        while (*curr++ != '\0');
    }
    envp[envCount] = NULL;

    status = start(argc, argv, envp);

    debug_CHECK(HeapFree(heap, 0, envp), RES != 0);
    cleanup_env:
    debug_CHECK(HeapFree(heap, 0, env), RES != 0);
    cleanup_environment_strings:
    debug_CHECK(FreeEnvironmentStringsW(envUtf16), RES != 0);
    cleanup_argv:
    debug_CHECK(HeapFree(heap, 0, argv), RES != 0);
    cleanup_cmdLine:
    debug_CHECK(HeapFree(heap, 0, cmdLine), RES != 0);
    cleanup_none:
    ExitProcess((uint32_t)status);
}

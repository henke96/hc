#if nativeGlue_DEBUG
    #define _nativeGlue_DEBUG_PRINT(STR) debug_print(STR)
#else
    #define _nativeGlue_DEBUG_PRINT(STR)
#endif

#define nativeGlue_LOOPER_ID 0

enum nativeGlue_cmdTag {
    nativeGlue_NO_CMD,
    nativeGlue_START,
    nativeGlue_RESUME,
    nativeGlue_SAVE_INSTANCE_STATE,
    nativeGlue_PAUSE,
    nativeGlue_STOP,
    nativeGlue_DESTROY, // Calling `nativeGlue_signalAppDone` not required, just return 0.
    nativeGlue_WINDOW_FOCUS_CHANGED,
    nativeGlue_NATIVE_WINDOW_CREATED,
    nativeGlue_NATIVE_WINDOW_RESIZED,
    nativeGlue_NATIVE_WINDOW_REDRAW_NEEDED,
    nativeGlue_NATIVE_WINDOW_DESTROYED,
    nativeGlue_INPUT_QUEUE_CREATED,
    nativeGlue_INPUT_QUEUE_DESTROYED,
    nativeGlue_CONTENT_RECT_CHANGED,
    nativeGlue_CONFIGURATION_CHANGED,
    nativeGlue_LOW_MEMORY
};

struct nativeGlue_cmd {
    enum nativeGlue_cmdTag tag;
    int32_t __pad;
    union {
        struct {
            void **outMallocPtr;
            uint64_t *outMallocSize;
        } saveInstanceState;

        struct {
            int32_t hasFocus;
            int32_t __pad;
        } windowFocusChanged;

        struct {
            void *window;
        } nativeWindowCreated;

        struct {
            void *window;
        } nativeWindowResized;

        struct {
            void *window;
        } nativeWindowRedrawNeeded;

        struct {
            void *window;
        } nativeWindowDestroyed;

        struct {
            void *inputQueue;
        } inputQueueCreated;

        struct {
            void *inputQueue;
        } inputQueueDestroyed;

        struct {
            const struct ARect *rect;
        } contentRectChanged;
    };
};

struct nativeGlue_appInfo {
    int32_t (*appThreadFunc)(void *looper, void *appThreadArg);
    void *appThreadArg;
};

struct nativeGlue {
    struct nativeGlue_appInfo appInfo;
    int32_t cmdPipe[2]; // [read, write]
    int64_t appPthread;
    int32_t appDoneFutex;
    int32_t __pad;
};

static struct nativeGlue _nativeGlue;

// Should be called by app after done with command.
static void nativeGlue_signalAppDone(void) {
    hc_ATOMIC_STORE(&_nativeGlue.appDoneFutex, 1, hc_ATOMIC_RELEASE);
    debug_CHECK(sys_futex(&_nativeGlue.appDoneFutex, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0), RES >= 0);
}

static inline void _nativeGlue_resetAppDone(void) {
    hc_ATOMIC_STORE(&_nativeGlue.appDoneFutex, 0, hc_ATOMIC_RELAXED);
}

static void _nativeGlue_waitAppDone(void) {
    for (;;) {
        if (hc_ATOMIC_LOAD(&_nativeGlue.appDoneFutex, hc_ATOMIC_ACQUIRE) == 1) break;
        debug_CHECK(sys_futex(&_nativeGlue.appDoneFutex, FUTEX_WAIT_PRIVATE, 0, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
    }
}

static void _nativeGlue_writeAndWait(struct nativeGlue_cmd *cmd) {
    _nativeGlue_resetAppDone();
    if (sys_write(_nativeGlue.cmdPipe[1], cmd, sizeof(*cmd)) != sizeof(*cmd)) debug_abort();
    _nativeGlue_waitAppDone();
}

static void _nativeGlue_onHelperNoArg(enum nativeGlue_cmdTag tag) {
    struct nativeGlue_cmd cmd;
    cmd.tag = tag;
    _nativeGlue_writeAndWait(&cmd);
}

static void _nativeGlue_onHelperPtrArg(enum nativeGlue_cmdTag tag, void *arg) {
    struct nativeGlue_cmd cmd;
    cmd.tag = tag;
    cmd.nativeWindowCreated.window = arg;
    _nativeGlue_writeAndWait(&cmd);
}

static void _nativeGlue_onStart(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onStart\n");
    _nativeGlue_onHelperNoArg(nativeGlue_START);
}

static void _nativeGlue_onResume(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onResume\n");
    _nativeGlue_onHelperNoArg(nativeGlue_RESUME);
}

static void *_nativeGlue_onSaveInstanceState(hc_UNUSED struct ANativeActivity *activity, uint64_t *outMallocSize) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onSaveInstanceState\n");
    void *savedState = NULL;
    struct nativeGlue_cmd cmd = {
        .tag = nativeGlue_SAVE_INSTANCE_STATE,
        .saveInstanceState = {
            .outMallocPtr = &savedState,
            .outMallocSize = outMallocSize
        }
    };
    _nativeGlue_writeAndWait(&cmd);
    return savedState;
}

static void _nativeGlue_onPause(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onPause\n");
    _nativeGlue_onHelperNoArg(nativeGlue_PAUSE);
}

static void _nativeGlue_onStop(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onStop\n");
    _nativeGlue_onHelperNoArg(nativeGlue_STOP);
}

static void _nativeGlue_onDestroy(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onDestroy\n");
    _nativeGlue_onHelperNoArg(nativeGlue_DESTROY);

    // Perform cleanup.
    int32_t i = 2;
    while (i--) debug_CHECK(sys_close(_nativeGlue.cmdPipe[i]), RES == 0);

    // Wait for app thread to exit.
    debug_CHECK(pthread_join(_nativeGlue.appPthread, NULL), RES == 0);
    debug_print("Application exited gracefully!\n");
}

static void _nativeGlue_onWindowFocusChanged(hc_UNUSED struct ANativeActivity *activity, int32_t hasFocus) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onWindowFocusChanged\n");
    struct nativeGlue_cmd cmd;
    cmd.tag = nativeGlue_WINDOW_FOCUS_CHANGED;
    cmd.windowFocusChanged.hasFocus = hasFocus;
    _nativeGlue_writeAndWait(&cmd);
}

static void _nativeGlue_onNativeWindowCreated(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onNativeWindowCreated\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_CREATED, window);
}

static void _nativeGlue_onNativeWindowResized(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onNativeWindowResized\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_RESIZED, window);
}

static void _nativeGlue_onNativeWindowRedrawNeeded(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onNativeWindowRedrawNeeded\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_REDRAW_NEEDED, window);
}

static void _nativeGlue_onNativeWindowDestroyed(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onNativeWindowDestroyed\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_DESTROYED, window);
}

static void _nativeGlue_onInputQueueCreated(hc_UNUSED struct ANativeActivity *activity, void *inputQueue) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onInputQueueCreated\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_INPUT_QUEUE_CREATED, inputQueue);
}

static void _nativeGlue_onInputQueueDestroyed(hc_UNUSED struct ANativeActivity *activity, void *inputQueue) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onInputQueueDestroyed\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_INPUT_QUEUE_DESTROYED, inputQueue);
}

static void _nativeGlue_onContentRectChanged(hc_UNUSED struct ANativeActivity *activity, const struct ARect *rect) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onContentRectChanged\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_CONTENT_RECT_CHANGED, (void *)rect);
}

static void _nativeGlue_onConfigurationChanged(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onConfigurationChanged\n");
    _nativeGlue_onHelperNoArg(nativeGlue_CONFIGURATION_CHANGED);
}

static void _nativeGlue_onLowMemory(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("_nativeGlue_onLowMemory\n");
    _nativeGlue_onHelperNoArg(nativeGlue_LOW_MEMORY);
}

static void *_nativeGlue_appThread(hc_UNUSED void *arg) {
    void *looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    debug_ASSERT(looper != NULL);
    if (ALooper_addFd(looper, _nativeGlue.cmdPipe[0], nativeGlue_LOOPER_ID, ALOOPER_EVENT_INPUT, NULL, NULL) != 1) debug_abort();

    int32_t status = _nativeGlue.appInfo.appThreadFunc(looper, _nativeGlue.appInfo.appThreadArg);
    if (status != 0) {
        debug_printNum("Application ran into error: ", status, "\n");
        debug_abort();
    }

    debug_CHECK(ALooper_removeFd(looper, _nativeGlue.cmdPipe[0]), RES == 1);

    nativeGlue_signalAppDone();
    return NULL;
}

static int32_t nativeGlue_init(struct ANativeActivity *activity, struct nativeGlue_appInfo *appInfo) {
    _nativeGlue.appInfo = *appInfo;

    int32_t status = sys_pipe2(&_nativeGlue.cmdPipe[0], O_DIRECT | O_NONBLOCK | O_CLOEXEC);
    if (status < 0) return -1;

    status = pthread_create(&_nativeGlue.appPthread, NULL, _nativeGlue_appThread, NULL);
    if (status != 0) {
        int32_t i = 2;
        while (i--) debug_CHECK(sys_close(_nativeGlue.cmdPipe[i]), RES == 0);
        return -2;
    }

    activity->callbacks->onStart = _nativeGlue_onStart;
    activity->callbacks->onResume = _nativeGlue_onResume;
    activity->callbacks->onSaveInstanceState = _nativeGlue_onSaveInstanceState;
    activity->callbacks->onPause = _nativeGlue_onPause;
    activity->callbacks->onStop = _nativeGlue_onStop;
    activity->callbacks->onDestroy = _nativeGlue_onDestroy;
    activity->callbacks->onWindowFocusChanged = _nativeGlue_onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = _nativeGlue_onNativeWindowCreated;
    activity->callbacks->onNativeWindowResized = _nativeGlue_onNativeWindowResized;
    activity->callbacks->onNativeWindowRedrawNeeded = _nativeGlue_onNativeWindowRedrawNeeded;
    activity->callbacks->onNativeWindowDestroyed = _nativeGlue_onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = _nativeGlue_onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = _nativeGlue_onInputQueueDestroyed;
    activity->callbacks->onContentRectChanged = _nativeGlue_onContentRectChanged;
    activity->callbacks->onConfigurationChanged = _nativeGlue_onConfigurationChanged;
    activity->callbacks->onLowMemory = _nativeGlue_onLowMemory;

    return 0;
}

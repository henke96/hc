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
            bool *requestPointerCapture;
        } windowFocusChanged;

        struct {
            void *window;
        } nativeWindowCreated;

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

static void _nativeGlue_onHelperPtrArg(enum nativeGlue_cmdTag tag, const void *arg) {
    struct nativeGlue_cmd cmd;
    cmd.tag = tag;
    // Abuse union a bit.
    cmd.contentRectChanged.rect = arg;
    _nativeGlue_writeAndWait(&cmd);
}

static void _nativeGlue_onStart(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onStart enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_START);
    _nativeGlue_DEBUG_PRINT("onStart leave\n");
}

static void _nativeGlue_onResume(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onResume enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_RESUME);
    _nativeGlue_DEBUG_PRINT("onResume leave\n");
}

static void *_nativeGlue_onSaveInstanceState(hc_UNUSED struct ANativeActivity *activity, uint64_t *outMallocSize) {
    _nativeGlue_DEBUG_PRINT("onSaveInstanceState enter\n");
    void *savedState = NULL;
    struct nativeGlue_cmd cmd = {
        .tag = nativeGlue_SAVE_INSTANCE_STATE,
        .saveInstanceState = {
            .outMallocPtr = &savedState,
            .outMallocSize = outMallocSize
        }
    };
    _nativeGlue_writeAndWait(&cmd);
    _nativeGlue_DEBUG_PRINT("onSaveInstanceState leave\n");
    return savedState;
}

static void _nativeGlue_onPause(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onPause enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_PAUSE);
    _nativeGlue_DEBUG_PRINT("onPause leave\n");
}

static void _nativeGlue_onStop(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onStop enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_STOP);
    _nativeGlue_DEBUG_PRINT("onStop leave\n");
}

static void _nativeGlue_onDestroy(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onDestroy enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_DESTROY);

    // Perform cleanup.
    int32_t i = 2;
    while (i--) debug_CHECK(sys_close(_nativeGlue.cmdPipe[i]), RES == 0);

    // Wait for app thread to exit.
    debug_CHECK(pthread_join(_nativeGlue.appPthread, NULL), RES == 0);
    debug_print("Application exited gracefully!\n");
    _nativeGlue_DEBUG_PRINT("onDestroy leave\n");
}

static void _nativeGlue_onWindowFocusChanged(struct ANativeActivity *activity, int32_t hasFocus) {
    _nativeGlue_DEBUG_PRINT("onWindowFocusChanged enter\n");
    bool requestPointerCapture = false;

    struct nativeGlue_cmd cmd;
    cmd.tag = nativeGlue_WINDOW_FOCUS_CHANGED;
    cmd.windowFocusChanged.hasFocus = hasFocus;
    cmd.windowFocusChanged.requestPointerCapture = &requestPointerCapture;
    _nativeGlue_writeAndWait(&cmd);

    if (requestPointerCapture) {
        struct jni_env **env = activity->env;
        void *activityObj = activity->activityObj;

        void *activityClass = (*env)->getObjectClass(env, activityObj);
        if (activityClass == NULL) debug_abort();

        void *nativeViewField = (*env)->getFieldId(env, activityClass, "mNativeContentView", "Landroid/app/NativeActivity$NativeContentView;");
        if (nativeViewField == NULL) debug_abort();

        void *nativeViewObj = (*env)->getObjectField(env, activityObj, nativeViewField);
        if (nativeViewObj == NULL) debug_abort();

        void *nativeViewClass = (*env)->getObjectClass(env, nativeViewObj);
        if (nativeViewClass == NULL) debug_abort();

        void *requestPointerCaptureMethod = (*env)->getMethodId(env, nativeViewClass, "requestPointerCapture", "()V");
        if (requestPointerCaptureMethod == NULL) debug_abort();

        (*env)->callVoidMethod(env, nativeViewObj, requestPointerCaptureMethod);
        _nativeGlue_DEBUG_PRINT("onWindowFocusChanged requested pointer capture\n");
    }
    _nativeGlue_DEBUG_PRINT("onWindowFocusChanged leave\n");
}

static void _nativeGlue_onNativeWindowCreated(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("onNativeWindowCreated enter\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_CREATED, window);
    _nativeGlue_DEBUG_PRINT("onNativeWindowCreated leave\n");
}

static void _nativeGlue_onNativeWindowRedrawNeeded(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("onNativeWindowRedrawNeeded enter\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_REDRAW_NEEDED, window);
    _nativeGlue_DEBUG_PRINT("onNativeWindowRedrawNeeded leave\n");
}

static void _nativeGlue_onNativeWindowDestroyed(hc_UNUSED struct ANativeActivity *activity, void *window) {
    _nativeGlue_DEBUG_PRINT("onNativeWindowDestroyed enter\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_NATIVE_WINDOW_DESTROYED, window);
    _nativeGlue_DEBUG_PRINT("onNativeWindowDestroyed leave\n");
}

static void _nativeGlue_onInputQueueCreated(hc_UNUSED struct ANativeActivity *activity, void *inputQueue) {
    _nativeGlue_DEBUG_PRINT("onInputQueueCreated enter\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_INPUT_QUEUE_CREATED, inputQueue);
    _nativeGlue_DEBUG_PRINT("onInputQueueCreated leave\n");
}

static void _nativeGlue_onInputQueueDestroyed(hc_UNUSED struct ANativeActivity *activity, void *inputQueue) {
    _nativeGlue_DEBUG_PRINT("onInputQueueDestroyed enter\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_INPUT_QUEUE_DESTROYED, inputQueue);
    _nativeGlue_DEBUG_PRINT("onInputQueueDestroyed leave\n");
}

static void _nativeGlue_onContentRectChanged(hc_UNUSED struct ANativeActivity *activity, const struct ARect *rect) {
    _nativeGlue_DEBUG_PRINT("onContentRectChanged enter\n");
    _nativeGlue_onHelperPtrArg(nativeGlue_CONTENT_RECT_CHANGED, rect);
    _nativeGlue_DEBUG_PRINT("onContentRectChanged leave\n");
}

static void _nativeGlue_onConfigurationChanged(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onConfigurationChanged enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_CONFIGURATION_CHANGED);
    _nativeGlue_DEBUG_PRINT("onConfigurationChanged leave\n");
}

static void _nativeGlue_onLowMemory(hc_UNUSED struct ANativeActivity *activity) {
    _nativeGlue_DEBUG_PRINT("onLowMemory enter\n");
    _nativeGlue_onHelperNoArg(nativeGlue_LOW_MEMORY);
    _nativeGlue_DEBUG_PRINT("onLowMemory leave\n");
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
    // Skip `onNativeWindowResized` as it is unreliable.
    activity->callbacks->onNativeWindowRedrawNeeded = _nativeGlue_onNativeWindowRedrawNeeded;
    activity->callbacks->onNativeWindowDestroyed = _nativeGlue_onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = _nativeGlue_onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = _nativeGlue_onInputQueueDestroyed;
    activity->callbacks->onContentRectChanged = _nativeGlue_onContentRectChanged;
    activity->callbacks->onConfigurationChanged = _nativeGlue_onConfigurationChanged;
    activity->callbacks->onLowMemory = _nativeGlue_onLowMemory;

    return 0;
}

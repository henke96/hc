struct ARect {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

struct ANativeActivity {
    struct {
        void (*onStart)(struct ANativeActivity *activity);
        void (*onResume)(struct ANativeActivity *activity);
        // Returned pointer must come from malloc.
        void *(*onSaveInstanceState)(struct ANativeActivity *activity, uint64_t *outMallocSize);
        void (*onPause)(struct ANativeActivity *activity);
        void (*onStop)(struct ANativeActivity *activity);
        void (*onDestroy)(struct ANativeActivity *activity);
        void (*onWindowFocusChanged)(struct ANativeActivity *activity, int32_t hasFocus);
        void (*onNativeWindowCreated)(struct ANativeActivity *activity, void *window);
        void (*onNativeWindowResized)(struct ANativeActivity *activity, void *window);
        void (*onNativeWindowRedrawNeeded)(struct ANativeActivity *activity, void *window);
        // Can not touch `window` after this returns.
        void (*onNativeWindowDestroyed)(struct ANativeActivity *activity, void *window);
        void (*onInputQueueCreated)(struct ANativeActivity *activity, void *inputQueue);
        void (*onInputQueueDestroyed)(struct ANativeActivity *activity, void *inputQueue);
        void (*onContentRectChanged)(struct ANativeActivity *activity, const struct ARect *rect);
        // New configuration can be retreived from `assetManager`.
        void (*onConfigurationChanged)(struct ANativeActivity *activity);
        void (*onLowMemory)(struct ANativeActivity *activity);
    } *callbacks;

    struct jni_invokeInterface **vm;
    struct jni_env **env;
    void *activityObj;

    const char *internalDataPath;
    const char *externalDataPath;

    int32_t sdkVersion;
    int32_t __pad;
    void *instance;
    void *assetManager;
    const char *obbPath;
};

// looper.h
#define ALOOPER_PREPARE_ALLOW_NON_CALLBACKS 1

#define ALOOPER_EVENT_INPUT 0x01
#define ALOOPER_EVENT_OUTPUT 0x02
#define ALOOPER_EVENT_ERROR 0x04
#define ALOOPER_EVENT_HANGUP 0x08
#define ALOOPER_EVENT_INVALID 0x10

#define ALOOPER_POLL_WAKE -1
#define ALOOPER_POLL_CALLBACK -2
#define ALOOPER_POLL_TIMEOUT -3
#define ALOOPER_POLL_ERROR -4

void *ALooper_prepare(int32_t flags);

int32_t ALooper_addFd(
    void *looper,
    int32_t fd,
    int32_t ident,
    int32_t events,
    int32_t (*callback)(int32_t fd, int32_t events, void *data),
    void *data
);

int32_t ALooper_removeFd(void *looper, int32_t fd);

int32_t ALooper_pollAll(int32_t timeoutMs, int32_t *outFd, int32_t *outEvents, void **outData);

// input.h
void AInputQueue_attachLooper(
    void *inputQueue,
    void *looper,
    int32_t ident,
    int32_t (*callback)(int32_t fd, int32_t events, void *data),
    void *data
);

void AInputQueue_detachLooper(void *inputQueue);
int32_t AInputQueue_getEvent(void *inputQueue, void **outEvent);
int32_t AInputQueue_preDispatchEvent(void *inputQueue, void *event);
void AInputQueue_finishEvent(void *inputQueue, void *event, int32_t handled);
int32_t AInputEvent_getType(const void *event);

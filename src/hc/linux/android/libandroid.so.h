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
#define AINPUT_EVENT_TYPE_KEY 1
#define AINPUT_EVENT_TYPE_MOTION 2
#define AINPUT_EVENT_TYPE_FOCUS 3
#define AINPUT_EVENT_TYPE_CAPTURE 4
#define AINPUT_EVENT_TYPE_DRAG 5
#define AINPUT_EVENT_TYPE_TOUCH_MODE 6

#define AINPUT_SOURCE_CLASS_MASK 0x000000ff
#define AINPUT_SOURCE_CLASS_NONE 0x00000000
#define AINPUT_SOURCE_CLASS_BUTTON 0x00000001
#define AINPUT_SOURCE_CLASS_POINTER 0x00000002
#define AINPUT_SOURCE_CLASS_NAVIGATION 0x00000004
#define AINPUT_SOURCE_CLASS_POSITION 0x00000008
#define AINPUT_SOURCE_CLASS_JOYSTICK 0x00000010

#define AINPUT_SOURCE_UNKNOWN 0x00000000
#define AINPUT_SOURCE_KEYBOARD (0x00000100 | AINPUT_SOURCE_CLASS_BUTTON)
#define AINPUT_SOURCE_DPAD (0x00000200 | AINPUT_SOURCE_CLASS_BUTTON)
#define AINPUT_SOURCE_GAMEPAD (0x00000400 | AINPUT_SOURCE_CLASS_BUTTON)
#define AINPUT_SOURCE_TOUCHSCREEN (0x00001000 | AINPUT_SOURCE_CLASS_POINTER)
#define AINPUT_SOURCE_MOUSE (0x00002000 | AINPUT_SOURCE_CLASS_POINTER)
#define AINPUT_SOURCE_STYLUS (0x00004000 | AINPUT_SOURCE_CLASS_POINTER)
#define AINPUT_SOURCE_BLUETOOTH_STYLUS (0x00008000 | AINPUT_SOURCE_STYLUS)
#define AINPUT_SOURCE_TRACKBALL (0x00010000 | AINPUT_SOURCE_CLASS_NAVIGATION)
#define AINPUT_SOURCE_MOUSE_RELATIVE (0x00020000 | AINPUT_SOURCE_CLASS_NAVIGATION)
#define AINPUT_SOURCE_TOUCHPAD (0x00100000 | AINPUT_SOURCE_CLASS_POSITION)
#define AINPUT_SOURCE_TOUCH_NAVIGATION (0x00200000 | AINPUT_SOURCE_CLASS_NONE)
#define AINPUT_SOURCE_JOYSTICK (0x01000000 | AINPUT_SOURCE_CLASS_JOYSTICK)
#define AINPUT_SOURCE_HDMI (0x02000000 | AINPUT_SOURCE_CLASS_BUTTON)
#define AINPUT_SOURCE_SENSOR (0x04000000 | AINPUT_SOURCE_CLASS_NONE)
#define AINPUT_SOURCE_ROTARY_ENCODER (0x00400000 | AINPUT_SOURCE_CLASS_NONE)
#define AINPUT_SOURCE_ANY 0xffffff00

#define AMOTION_EVENT_ACTION_MASK = 0xff,
#define AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT 8
#define AMOTION_EVENT_ACTION_POINTER_INDEX_MASK 0xff00
#define AMOTION_EVENT_ACTION_DOWN 0
#define AMOTION_EVENT_ACTION_UP 1
#define AMOTION_EVENT_ACTION_MOVE 2
#define AMOTION_EVENT_ACTION_CANCEL 3
#define AMOTION_EVENT_ACTION_OUTSIDE 4
// Bits in AMOTION_EVENT_ACTION_POINTER_INDEX_MASK indicate what pointer.
#define AMOTION_EVENT_ACTION_POINTER_DOWN 5
#define AMOTION_EVENT_ACTION_POINTER_UP 6
#define AMOTION_EVENT_ACTION_HOVER_MOVE 7
#define AMOTION_EVENT_ACTION_SCROLL 8
#define AMOTION_EVENT_ACTION_HOVER_ENTER 9
#define AMOTION_EVENT_ACTION_HOVER_EXIT 10
#define AMOTION_EVENT_ACTION_BUTTON_PRESS 11
#define AMOTION_EVENT_ACTION_BUTTON_RELEASE 12

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
int32_t AInputEvent_getSource(const void *event);
int32_t AMotionEvent_getAction(const void *event);
float AMotionEvent_getRawX(const void *event, uint64_t pointerIndex);
float AMotionEvent_getRawY(const void *event, uint64_t pointerIndex);

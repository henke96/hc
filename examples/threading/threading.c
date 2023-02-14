#include "hc/hc.h"
#include "hc/util.c"
#include "hc/libc/small.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone_func.c"
#include "hc/linux/helpers/sys_clone3_func.c"

// 1 to use spin lock, 0 for futex-based locking.
#define SPIN 0

#define UNLOCKED 0
#define LOCKED 1

static char stack1[4096] hc_ALIGNED(16);
static char stack2[4096] hc_ALIGNED(16);

static int32_t printLock = UNLOCKED;
static int32_t child1Done = 1;
static int32_t child2Done = 1;

static void takeLock(void) {
    for (;;) {
        if (hc_ATOMIC_EXCHANGE(&printLock, LOCKED, hc_ATOMIC_ACQUIRE) == UNLOCKED) break;

#if SPIN
        while (hc_ATOMIC_LOAD(&printLock, hc_ATOMIC_RELAXED) == LOCKED) hc_ATOMIC_PAUSE;
#else
        debug_CHECK(sys_futex(&printLock, FUTEX_WAIT_PRIVATE, LOCKED, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
#endif
    }
}

static void releaseLock(void) {
    hc_ATOMIC_STORE(&printLock, UNLOCKED, hc_ATOMIC_RELEASE);

#if !SPIN
    debug_CHECK(sys_futex(&printLock, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0), RES >= 0);
#endif
}

static noreturn void thread(void *arg) {
    for (int32_t i = 0; i < 1000000; ++i) {
        takeLock();
        debug_printNum((char *)arg, i, "\n");
        releaseLock();
    }
    sys_exit(0);
}

static void waitChild(int32_t *futex) {
    for (;;) {
        if (hc_ATOMIC_LOAD(futex, hc_ATOMIC_ACQUIRE) == 0) break;
        debug_CHECK(sys_futex(futex, FUTEX_WAIT, 1, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
    }
}

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv) {
    uint32_t flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM | CLONE_CHILD_CLEARTID;
    // Start child1 thread.
    struct clone_args args = {
        .flags = flags,
        .stack = &stack1[0],
        .stack_size = sizeof(stack1),
        .child_tid = &child1Done
    };
    int32_t ret = sys_clone3_func(&args, sizeof(args), thread, (void *)"child1 ");
    if (ret < 0) return 1;

    // Start child2 thread.
    ret = sys_clone_func(flags, &stack2[sizeof(stack2)], NULL, 0, &child2Done, thread, (void *)"child2 ");
    if (ret < 0) return 1;

    for (int32_t i = 0; i < 1000000; ++i) {
        takeLock();
        debug_printNum("parent ", i, "\n");
        releaseLock();
    }

    waitChild(&child1Done);
    waitChild(&child2Done);
    return 0;
}

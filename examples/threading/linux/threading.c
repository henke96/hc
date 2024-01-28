#include "hc/hc.h"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone_func.c"
#include "hc/linux/helpers/sys_clone3_func.c"

#define ITERATIONS 10000000

#define UNLOCKED 0
#define LOCKED 1

static char stack1[4096] hc_ALIGNED(16);
static char stack2[4096] hc_ALIGNED(16);

static bool spin = false;
static int32_t lock = UNLOCKED;
static int64_t counter = 0;
static int32_t child1Done = 1;
static int32_t child2Done = 1;

static void takeLock(void) {
    for (;;) {
        if (hc_ATOMIC_EXCHANGE(&lock, LOCKED, hc_ATOMIC_ACQUIRE) == UNLOCKED) break;

        if (spin) while (hc_ATOMIC_LOAD(&lock, hc_ATOMIC_RELAXED) == LOCKED) hc_ATOMIC_PAUSE;
        else debug_CHECK(sys_futex(&lock, FUTEX_WAIT_PRIVATE, LOCKED, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
    }
}

static void releaseLock(void) {
    hc_ATOMIC_STORE(&lock, UNLOCKED, hc_ATOMIC_RELEASE);

    if (!spin) debug_CHECK(sys_futex(&lock, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0), RES >= 0);
}

static noreturn void thread(void *arg) {
    for (int32_t i = 0; i < ITERATIONS; ++i) {
        takeLock();
        ++counter;
        releaseLock();
    }
    takeLock();
    debug_print(arg);
    debug_print(" done!\n");
    releaseLock();
    sys_exit(0);
}

static void waitChild(int32_t *futex) {
    for (;;) {
        if (hc_ATOMIC_LOAD(futex, hc_ATOMIC_ACQUIRE) == 0) break;
        debug_CHECK(sys_futex(futex, FUTEX_WAIT, 1, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
    }
}

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    if (argc >= 2 && util_cstrCmp(argv[1], "--spin") == 0) spin = true;
    uint32_t flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM | CLONE_CHILD_CLEARTID;
    // Start child1 thread.
    struct clone_args args = {
        .flags = flags,
        .stack = &stack1[0],
        .stack_size = sizeof(stack1),
        .child_tid = &child1Done
    };
    int32_t ret = sys_clone3_func(&args, sizeof(args), thread, (void *)"child1");
    if (ret < 0) return 1;

    // Start child2 thread.
    ret = sys_clone_func(flags, &stack2[sizeof(stack2)], NULL, 0, &child2Done, thread, (void *)"child2");
    if (ret < 0) return 1;

    for (int32_t i = 0; i < ITERATIONS; ++i) {
        takeLock();
        ++counter;
        releaseLock();
    }
    takeLock();
    debug_print("Parent done!\n");
    releaseLock();

    waitChild(&child1Done);
    waitChild(&child2Done);
    debug_printNum("Counter: ", counter, "\n");
    return 0;
}

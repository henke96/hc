#include "hc/hc.h"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"
#include "hc/windows/_start.c"

#define ITERATIONS 10000000

#define UNLOCKED 0
#define LOCKED 1

static bool spin = false;
static int32_t lock = UNLOCKED;
static int64_t counter = 0;

static void takeLock(void) {
    for (;;) {
        if (hc_ATOMIC_EXCHANGE(&lock, LOCKED, hc_ATOMIC_ACQUIRE) == UNLOCKED) break;

        if (spin) while (hc_ATOMIC_LOAD(&lock, hc_ATOMIC_RELAXED) == LOCKED) hc_ATOMIC_PAUSE;
        else debug_CHECK(WaitOnAddress(&lock, &(int32_t) { LOCKED }, sizeof(lock), INFINITE), RES == 1);
    }
}

static void releaseLock(void) {
    hc_ATOMIC_STORE(&lock, UNLOCKED, hc_ATOMIC_RELEASE);

    if (!spin) WakeByAddressSingle(&lock);
}

static uint32_t thread(void *arg) {
    for (int32_t i = 0; i < ITERATIONS; ++i) {
        takeLock();
        ++counter;
        releaseLock();
    }
    takeLock();
    debug_print(arg);
    debug_print(" done!\n");
    releaseLock();
    return 0;
}

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    if (argc >= 2 && util_cstrCmp(argv[1], "--spin") == 0) spin = true;
    void *child1Handle = CreateThread(NULL, 0, thread, (void *)"child1", 0, NULL);
    if (child1Handle == NULL) return 1;

    void *child2Handle = CreateThread(NULL, 0, thread, (void *)"child2", 0, NULL);
    if (child2Handle == NULL) return 1;

    for (int32_t i = 0; i < ITERATIONS; ++i) {
        takeLock();
        ++counter;
        releaseLock();
    }
    takeLock();
    debug_print("Parent done!\n");
    releaseLock();

    debug_CHECK(WaitForSingleObjectEx(child1Handle, INFINITE, 0), RES == 0);
    debug_CHECK(WaitForSingleObjectEx(child2Handle, INFINITE, 0), RES == 0);
    debug_printNum("Counter: ", counter, "\n");
    return 0;
}

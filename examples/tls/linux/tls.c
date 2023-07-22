#include "hc/hc.h"
#include "hc/elf.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/tls.c"
#include "hc/linux/helpers/_start.c"
#include "hc/linux/helpers/sys_clone3_func.c"

static thread_local int32_t test1 = 1;
static thread_local char test2 = 2;
static thread_local int64_t test3 = 3;
static thread_local int16_t test4;
static thread_local int32_t test5;
static thread_local int64_t test6 = 6;

// For the main thread we use this array as tls area.
static char tlsArea[tls_AREA_SIZE(64)] hc_ALIGNED(8);

static int32_t childDone = 1;

static noreturn void thread(void *arg) {
    test4 = -4;
    test5 = -5;
    test6 = (int64_t)arg;

    debug_printNum("child test1 = ", test1, "\n");
    debug_printNum("child test2 = ", test2, "\n");
    debug_printNum("child test3 = ", test3, "\n");
    debug_printNum("child test4 = ", test4, "\n");
    debug_printNum("child test5 = ", test5, "\n");
    debug_printNum("child test6 = ", test6, "\n");
    debug_printNum("child &test1 = ", (int64_t)&test1, "\n");
    debug_printNum("child &test2 = ", (int64_t)&test2, "\n");
    debug_printNum("child &test3 = ", (int64_t)&test3, "\n");
    debug_printNum("child &test4 = ", (int64_t)&test4, "\n");
    debug_printNum("child &test5 = ", (int64_t)&test5, "\n");
    debug_printNum("child &test6 = ", (int64_t)&test6, "\n");

    sys_exit(0);
}

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    uint64_t *auxv = util_getAuxv(envp);

    struct elf_programHeader *tlsProgramHeader = tls_findProgramHeader(auxv);
    if (tlsProgramHeader == NULL) return 1;

    // Setup tls for main thread.
    {
        uint64_t threadPointer = tls_initArea(tlsProgramHeader, &tlsArea[0]);
        tls_setThreadPointer(threadPointer);
        debug_printNum("parent thread pointer: ", (int64_t)threadPointer, "\n");
        test4 = 4;
        test5 = 5;
    }

    // Start child thread with tls.
    {
        // Allocate stack and tls area for child thread.
        int64_t stackSize = 4096; // Tls area must be aligned to at least `tlsProgramHeader->segmentAlignment`,
                                  // so we make the stack size a nice power of 2 at put the tls area right after it.
        int64_t allocSize = stackSize + (int64_t)tls_AREA_SIZE(tlsProgramHeader->segmentMemorySize);
        char *threadArea = sys_mmap(NULL, allocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
        if ((int64_t)threadArea < 0) return 2;

        uint64_t threadPointer = tls_initArea(tlsProgramHeader, &threadArea[stackSize]);
        debug_printNum("child thread pointer: ", (int64_t)threadPointer, "\n");
        struct clone_args args = {
            .flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS | CLONE_CHILD_CLEARTID,
            .tls = threadPointer,
            .stack = threadArea,
            .stack_size = stackSize,
            .child_tid = &childDone
        };
        int32_t ret = sys_clone3_func(&args, sizeof(args), thread, (void *)-6);
        if (ret < 0) return 3;
    }

    // Wait for child to finish.
    for (;;) {
        if (hc_ATOMIC_LOAD(&childDone, hc_ATOMIC_ACQUIRE) == 0) break;
        debug_CHECK(sys_futex(&childDone, FUTEX_WAIT, 1, NULL, NULL, 0), RES == 0 || RES == -EAGAIN);
    }

    debug_printNum("parent test1 = ", test1, "\n");
    debug_printNum("parent test2 = ", test2, "\n");
    debug_printNum("parent test3 = ", test3, "\n");
    debug_printNum("parent test4 = ", test4, "\n");
    debug_printNum("parent test5 = ", test5, "\n");
    debug_printNum("parent test6 = ", test6, "\n");
    debug_printNum("parent &test1 = ", (int64_t)&test1, "\n");
    debug_printNum("parent &test2 = ", (int64_t)&test2, "\n");
    debug_printNum("parent &test3 = ", (int64_t)&test3, "\n");
    debug_printNum("parent &test4 = ", (int64_t)&test4, "\n");
    debug_printNum("parent &test5 = ", (int64_t)&test5, "\n");
    debug_printNum("parent &test6 = ", (int64_t)&test6, "\n");

    return 0;
}

#include "hc/hc.h"
#include "hc/elf.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"
#include "hc/windows/tls.c"
#include "hc/windows/_start.c"

static thread_local int32_t test1 = 1;
static thread_local char test2 = 2;
static thread_local int64_t test3 = 3;
static thread_local int16_t test4;
static thread_local int32_t test5;
static thread_local int64_t test6 = 6;

static uint32_t thread(void *arg) {
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

    return 0;
}

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    test4 = 4;
    test5 = 5;

    void *threadHandle = CreateThread(NULL, 0, thread, (void *)-6, 0, NULL);
    if (threadHandle == NULL) return 1;

    // Wait for child to finish.
    debug_CHECK(WaitForSingleObjectEx(threadHandle, INFINITE, 0), RES == 0);

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

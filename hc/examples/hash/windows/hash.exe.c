#include "hc/hc.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mem.c"
#include "hc/windows/windows.h"
#include "hc/windows/util.c"
#include "hc/debug.c"
#include "hc/windows/_start.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/sha1.c"

#include "../common.c"

static int32_t openStream(char *file, util_STREAM_T *stream) {
    void *heap = GetProcessHeap();
    if (heap == NULL) return -1;

    int32_t utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        file, -1,
        NULL, 0
    );
    if (utf16Count <= 0) return -1;

    uint16_t *utf16 = HeapAlloc(heap, 0, (uint64_t)utf16Count * sizeof(uint16_t));
    if (utf16 == NULL) return -1;

    utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        file, -1,
        utf16, utf16Count
    );
    if (utf16Count <= 0) {
        debug_CHECK(HeapFree(heap, 0, utf16), RES != 0);
        return -1;
    }

    *stream = CreateFileW(
        utf16,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    debug_CHECK(HeapFree(heap, 0, utf16), RES != 0);
    if (*stream == INVALID_HANDLE_VALUE) return -1;

    return 0;
}

static void closeStream(util_STREAM_T stream) {
    debug_CHECK(CloseHandle(stream), RES != 0);
}

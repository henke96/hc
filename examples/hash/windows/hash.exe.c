#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mem.c"
#include "hc/windows/windows.h"
#include "hc/windows/util.c"
#include "hc/windows/debug.c"
#include "hc/windows/_start.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/sha1.c"

#include "../common.c"

static void *fileHandle;

static int32_t init(char *file) {
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

    fileHandle = CreateFileW(
        utf16,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    debug_CHECK(HeapFree(heap, 0, utf16), RES != 0);
    if (fileHandle == INVALID_HANDLE_VALUE) return -1;

    return 0;
}

static void deinit(void) {
    debug_CHECK(CloseHandle(fileHandle), RES != 0);
}

static int32_t readIntoBuffer(void) {
    uint32_t numRead;
    if (ReadFile(fileHandle, &hash_buffer[0], sizeof(hash_buffer), &numRead, NULL) == 0) return -1;
    return (int32_t)numRead;
}

static int32_t printBuffer(int32_t size) {
    void *outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    return util_writeAll(outHandle, &hash_buffer[0], size);
}

#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compiler_rt/libc.c"
#include "hc/windows/windows.h"
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
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY,
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
    uint32_t read;
    if (ReadFile(fileHandle, &buffer[0], sizeof(buffer), &read, NULL) == 0) return -1;
    return (int32_t)read;
}

static int32_t printBuffer(int32_t size) {
    void *stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    uint32_t written;
    if (
        WriteFile(stdOutHandle, &buffer[0], (uint32_t)size, &written, NULL) == 0 ||
        written != (uint32_t)size
    ) return -1;

    return 0;
}

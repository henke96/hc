#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/libc/small.c"
#include "hc/windows/windows.h"
#include "hc/windows/debug.c"
#include "hc/windows/heap.c"
#include "hc/windows/_start.c"
#include "hc/allocator.c"

#include "../common.c"

static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceLen, char *path, int32_t pathLen, bool asBase64) {
    // Convert path to null terminated UTF-16.
    int32_t utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        path, pathLen,
        NULL, 0
    );
    if (utf16Count <= 0) return -1;

    uint16_t *pathZ = &alloc.mem[bufferLen];
    if (allocator_resize(&alloc, bufferLen + (utf16Count + 1) * (int64_t)sizeof(uint16_t)) < 0) return -2;

    utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        path, pathLen,
        pathZ, utf16Count
    );
    if (utf16Count <= 0) return -3;
    pathZ[utf16Count] = u'\0';

    // Open file and get size.
    void *pathHandle = CreateFileW(
        pathZ,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (pathHandle == INVALID_HANDLE_VALUE) return -4;

    int32_t status;

    int64_t contentLen;
    if (GetFileSizeEx(pathHandle, &contentLen) == 0) {
        status = -5;
        goto cleanup_pathHandle;
    }

    // Resize buffer.
    int64_t insertLen = contentLen;
    if (asBase64) insertLen = base64_ENCODE_SIZE(contentLen);

    int64_t newBufferLen = bufferLen + (insertLen - replaceLen);
    if (allocator_resize(&alloc, newBufferLen) < 0) {
        status = -6;
        goto cleanup_pathHandle;
    }

    // Move existing content to make room.
    hc_MEMMOVE(
        &alloc.mem[replaceIndex + insertLen],
        &alloc.mem[replaceIndex + replaceLen],
        (uint64_t)(bufferLen - (replaceLen + replaceIndex))
    );
    bufferLen = newBufferLen;

    // Read content into buffer.
    char *content = &alloc.mem[replaceIndex + insertLen - contentLen];
    int64_t remaining = contentLen;
    while (remaining > 0) {
        int64_t index = contentLen - remaining;
        uint32_t toRead = UINT32_MAX;
        if (remaining < UINT32_MAX) toRead = (uint32_t)remaining;

        uint32_t read;
        if (ReadFile(pathHandle, &content[index], toRead, &read, NULL) == 0) {
            status = -7;
            goto cleanup_pathHandle;
        }
        remaining -= read;
    }
    // Verify we are at end of file.
    char eofTest;
    uint32_t eofRead;
    if (
        ReadFile(pathHandle, &eofTest, 1, &eofRead, NULL) == 0 ||
        eofRead != 0
    ) {
        status = -8;
        goto cleanup_pathHandle;
    }

    // Convert to base 64 if requested. Can be done in place as we put content at end of insert gap.
    if (asBase64) base64_encode(&alloc.mem[replaceIndex], &content[0], contentLen);

    status = 0;
    cleanup_pathHandle:
    debug_CHECK(CloseHandle(pathHandle), RES != 0);
    return status;
}

static int32_t writeToFile(char *path, char *content, int64_t contentLen) {
    int32_t utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        path, -1,
        NULL, 0
    );
    if (utf16Count <= 0) return -1;

    uint16_t *pathZ = &alloc.mem[alloc.size];
    if (allocator_resize(&alloc, alloc.size + utf16Count * (int64_t)sizeof(uint16_t)) < 0) return -2;

    utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        path, -1,
        pathZ, utf16Count
    );
    if (utf16Count <= 0) return -3;

    void *pathHandle = CreateFileW(
        pathZ,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (pathHandle == INVALID_HANDLE_VALUE) return -4;

    int32_t status;
    int64_t remaining = contentLen;
    while (remaining > 0) {
        int64_t index = contentLen - remaining;
        uint32_t toWrite = UINT32_MAX;
        if (remaining < UINT32_MAX) toWrite = (uint32_t)remaining;

        uint32_t written;
        if (WriteFile(pathHandle, &content[index], toWrite, &written, NULL) == 0) {
            status = -5;
            goto cleanup_pathHandle;
        }
        remaining -= written;
    }

    status = 0;
    cleanup_pathHandle:
    debug_CHECK(CloseHandle(pathHandle), RES != 0);
    return status;
}

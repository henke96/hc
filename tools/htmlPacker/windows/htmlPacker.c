#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/windows/windows.h"
#include "hc/windows/util.c"
#include "hc/windows/debug.c"
#include "hc/windows/heap.c"
#include "hc/windows/_start.c"
static struct SYSTEMINFO systemInfo;
#define allocator_PAGE_SIZE systemInfo.pageSize
#include "hc/allocator.c"

#include "../common.c"

static void initPageSize(hc_UNUSED char **envp) {
    GetSystemInfo(&systemInfo);
}

// Convert utf8 (optionally null terminated if `utf8Length` is -1) into null terminated utf16.
static uint16_t *utf8ToUtf16(char *utf8, int32_t utf8Length) {
    int32_t utf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8, utf8Length,
        NULL, 0
    );
    if (utf16Count <= 0) return NULL;

    int64_t alignedAllocOffset = math_ALIGN_FORWARD(alloc.size, 2);
    uint16_t *utf16Z = &alloc.mem[alignedAllocOffset];
    int32_t utf16ZCount = utf16Count + (utf8Length != -1);
    if (allocator_resize(&alloc, alignedAllocOffset + utf16ZCount * (int64_t)sizeof(uint16_t)) < 0) return NULL;

    if (
        MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            utf8, utf8Length,
            utf16Z, utf16Count
        ) != utf16Count
    ) return NULL;
    utf16Z[utf16ZCount - 1] = u'\0';
    return utf16Z;
}

static int32_t changeDir(char *path) {
    uint16_t *pathZ = utf8ToUtf16(path, -1);
    if (pathZ == NULL) return -1;

    if (!SetCurrentDirectoryW(pathZ)) return -1;
    return 0;
}

static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceLen, char *path, int32_t pathLen, bool asBase64) {
    uint16_t *pathZ = utf8ToUtf16(path, pathLen);
    if (pathZ == NULL) return -1;

    // Open file and get size.
    void *pathHandle = CreateFileW(
        pathZ,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (pathHandle == INVALID_HANDLE_VALUE) return -2;

    int32_t status;

    int64_t contentLen;
    if (GetFileSizeEx(pathHandle, &contentLen) == 0) {
        status = -3;
        goto cleanup_pathHandle;
    }

    // Resize buffer.
    int64_t insertLen = contentLen;
    if (asBase64) insertLen = base64_ENCODE_SIZE(contentLen);

    int64_t newBufferLen = bufferLen + (insertLen - replaceLen);
    if (allocator_resize(&alloc, newBufferLen) < 0) {
        status = -4;
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
            status = -5;
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
        status = -6;
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
    uint16_t *pathZ = utf8ToUtf16(path, -1);
    if (pathZ == NULL) return -1;

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

    int32_t status = util_writeAll(pathHandle, content, contentLen);
    if (status < 0) {
        status = -5;
        goto cleanup_pathHandle;
    }

    cleanup_pathHandle:
    debug_CHECK(CloseHandle(pathHandle), RES != 0);
    return status;
}

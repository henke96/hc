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

// Convert utf8 (optionally null terminated if `utf8Length` is -1) into null terminated utf16.
static uint16_t *utf8ToUtf16(char *utf8, int32_t utf8Length, int32_t *utf16Count) {
    int32_t count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8, utf8Length,
        NULL, 0
    );
    if (count <= 0) return NULL;

    int64_t alignedAllocOffset = math_ALIGN_FORWARD(htmlPacker_alloc.size, 2);
    uint16_t *utf16Z = &htmlPacker_alloc.mem[alignedAllocOffset];
    *utf16Count = count + (utf8Length != -1);
    if (allocator_resize(&htmlPacker_alloc, alignedAllocOffset + *utf16Count * (int64_t)sizeof(uint16_t)) < 0) return NULL;

    if (
        MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            utf8, utf8Length,
            utf16Z, count
        ) != count
    ) return NULL;
    utf16Z[*utf16Count - 1] = u'\0';
    return utf16Z;
}

static void initPageSize(hc_UNUSED char **envp) {
    GetSystemInfo(&systemInfo);
}

static int32_t init(char **includePaths) {
    int64_t i = 0;
    for (; includePaths[i] != NULL; ++i) {
        if (allocator_resize(&htmlPacker_alloc, (i + 1) * (int64_t)sizeof(void *)) < 0) return -1;

        int32_t utf16Count;
        uint16_t *pathZ = utf8ToUtf16(includePaths[i], -1, &utf16Count);
        if (pathZ == NULL) return -2;

        void *handle = CreateFileW(
            pathZ,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
        );
        if (handle == INVALID_HANDLE_VALUE) return -3;
        ((void **)htmlPacker_alloc.mem)[i] = handle;
    }
    htmlPacker_buffer = (void *)&((void **)htmlPacker_alloc.mem)[i];
    return 0;
}

static void deinit(void) {
    for (void **includePathHandle = htmlPacker_alloc.mem; (char *)includePathHandle != htmlPacker_buffer; ++includePathHandle) {
        debug_CHECK(CloseHandle(*includePathHandle), RES == 1);
    }
}

static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceSize, char *path, int32_t pathLen, bool asBase64) {
    int32_t utf16Count;
    uint16_t *pathZ = utf8ToUtf16(path, pathLen, &utf16Count);
    if (pathZ == NULL || utf16Count > INT16_MAX) return -1;

    // Find and open path relative to include paths.
    void *pathHandle;
    for (void **includePathHandle = htmlPacker_alloc.mem; (char *)includePathHandle != htmlPacker_buffer; ++includePathHandle) {
        struct IO_STATUS_BLOCK ioStatusBlock;
        uint16_t pathUtf16Size = (uint16_t)(utf16Count - 1) * sizeof(uint16_t); // Without null terminator.
        struct UNICODE_STRING pathUnicodeString = {
            .length = pathUtf16Size,
            .maximumLength = pathUtf16Size,
            .buffer = pathZ
        };
        struct OBJECT_ATTRIBUTES objectAttributes = {
            .length = sizeof(objectAttributes),
            .rootDirectory = *includePathHandle,
            .objectName = &pathUnicodeString,
            .attributes = 0,
            .securityDescriptor = NULL,
            .securityQualityOfService = NULL
        };
        int32_t status = NtCreateFile(
            &pathHandle,
            READ_CONTROL | FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | SYNCHRONIZE,
            &objectAttributes,
            &ioStatusBlock,
            NULL,
            0,
            FILE_SHARE_READ,
            FILE_OPEN,
            FILE_SYNCHRONOUS_IO_NONALERT,
            NULL,
            0
        );
        if (NT_SUCCESS(status)) goto foundPath;
    }
    return -2;
    foundPath:;

    // Get file size.
    int32_t status;
    int64_t contentSize;
    if (GetFileSizeEx(pathHandle, &contentSize) != 1) {
        status = -3;
        goto cleanup_pathHandle;
    }

    // Resize buffer.
    int64_t insertSize = contentSize;
    if (asBase64) insertSize = base64_ENCODE_SIZE(contentSize);

    // Allocate an extra byte to be able to verify EOF when reading.
    int64_t newBufferSize = 1 + htmlPacker_bufferSize + (insertSize - replaceSize);
    if (allocator_resize(&htmlPacker_alloc, &htmlPacker_buffer[newBufferSize] - (char *)htmlPacker_alloc.mem) < 0) {
        status = -4;
        goto cleanup_pathHandle;
    }

    // Move existing content to make room.
    hc_MEMMOVE(
        &htmlPacker_buffer[replaceIndex + insertSize],
        &htmlPacker_buffer[replaceIndex + replaceSize],
        (uint64_t)(htmlPacker_bufferSize - (replaceSize + replaceIndex))
    );
    htmlPacker_bufferSize = newBufferSize;

    // Read content into buffer, and verify EOF by attempting to read an extra byte.
    char *content = &htmlPacker_buffer[replaceIndex + insertSize - contentSize];
    if (util_readAll(pathHandle, content, contentSize + 1) != contentSize) {
        status = -5;
        goto cleanup_pathHandle;
    }

    // Convert to base 64 if requested. Can be done in place as we put content at end of insert gap.
    if (asBase64) base64_encode(&htmlPacker_buffer[replaceIndex], &content[0], contentSize);

    status = 0;
    cleanup_pathHandle:
    debug_CHECK(CloseHandle(pathHandle), RES == 1);
    return status;
}

static int32_t writeToFile(char *path, char *content, int64_t contentSize) {
    int32_t utf16Count;
    uint16_t *pathZ = utf8ToUtf16(path, -1, &utf16Count);
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
    if (pathHandle == INVALID_HANDLE_VALUE) return -2;

    int32_t status = util_writeAll(pathHandle, content, contentSize);
    debug_CHECK(CloseHandle(pathHandle), RES != 0);
    return status;
}

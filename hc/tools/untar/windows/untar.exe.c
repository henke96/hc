#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/windows/windows.h"
#include "hc/windows/util.c"
#include "hc/windows/debug.c"
#include "hc/windows/_start.c"
#include "hc/tar.h"

#include "../common.c"

static void *inputHandle;
static void *fileHandle;
static void *cwdHandle;
static uint16_t utf16Path[MAX_PATH];

static int32_t openInput(char *path) {
    if (path[0] == '-' && path[1] == '\0') {
        inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    } else {
        if (
            MultiByteToWideChar(
                CP_UTF8, MB_ERR_INVALID_CHARS,
                path, -1,
                &utf16Path[0], hc_ARRAY_LEN(utf16Path)
            ) <= 0
        ) return -1;
        inputHandle = CreateFileW(
            &utf16Path[0],
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );
    }
    if (inputHandle == INVALID_HANDLE_VALUE) return -1;
    if (GetCurrentDirectoryW(hc_ARRAY_LEN(utf16Path), &utf16Path[0]) == 0) goto cleanup_inputHandle;
    cwdHandle = CreateFileW(
        &utf16Path[0],
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (cwdHandle == INVALID_HANDLE_VALUE) goto cleanup_inputHandle;
    return 0;
    cleanup_inputHandle:
    debug_CHECK(CloseHandle(inputHandle), RES != 0);
    return -1;
}

static int32_t readInput(void) {
    if (util_readAll(inputHandle, &buffer[0], sizeof(buffer)) != sizeof(buffer)) return -1;
    return 0;
}

static void closeInput(void) {
    debug_CHECK(CloseHandle(cwdHandle), RES != 0);
    debug_CHECK(CloseHandle(inputHandle), RES != 0);
}

static int32_t _create(char *prefix, char *name, bool directory) {
    void *prefixHandle = cwdHandle;
    if (prefix[0] != '\0') {
        if (
            MultiByteToWideChar(
                CP_UTF8, MB_ERR_INVALID_CHARS,
                prefix, -1,
                &utf16Path[0], hc_ARRAY_LEN(utf16Path)
            ) <= 0
        ) return -1;

        prefixHandle = CreateFileW(
            &utf16Path[0],
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
        );
        if (prefixHandle == INVALID_HANDLE_VALUE) return -1;
    }

    int32_t status = -1;
    int32_t nameUtf16Count = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        name, -1,
        &utf16Path[0], hc_ARRAY_LEN(utf16Path)
    );
    if (nameUtf16Count <= 0) goto cleanup_prefixHandle;

    struct IO_STATUS_BLOCK ioStatusBlock;
    uint16_t nameUtf16Size = (uint16_t)(nameUtf16Count - 1) * sizeof(uint16_t); // Without null terminator.
    struct UNICODE_STRING nameUnicodeString = {
        .length = nameUtf16Size,
        .maximumLength = nameUtf16Size,
        .buffer = &utf16Path[0]
    };
    struct OBJECT_ATTRIBUTES objectAttributes = {
        .length = sizeof(objectAttributes),
        .rootDirectory = prefixHandle,
        .objectName = &nameUnicodeString,
        .attributes = 0,
        .securityDescriptor = NULL,
        .securityQualityOfService = NULL
    };
    void *handle;
    status = NtCreateFile(
        &handle,
        GENERIC_WRITE | SYNCHRONIZE,
        &objectAttributes,
        &ioStatusBlock,
        NULL,
        0,
        0,
        FILE_CREATE,
        FILE_SYNCHRONOUS_IO_NONALERT | (directory ? FILE_DIRECTORY_FILE : 0),
        NULL,
        0
    );
    if (status >= 0) {
        if (directory) debug_CHECK(CloseHandle(handle), RES != 0);
        else fileHandle = handle;
    }

    cleanup_prefixHandle:;
    if (prefixHandle != cwdHandle) debug_CHECK(CloseHandle(prefixHandle), RES != 0);
    return status;
}

static int32_t createDir(char *prefix, char *name) {
    return _create(prefix, name, true);
}

static int32_t createFile(char *prefix, char *name) {
    return _create(prefix, name, false);
}

static int32_t writeToFile(int32_t size) {
    return util_writeAll(fileHandle, &buffer[0], size);
}

static void closeFile(void) {
    debug_CHECK(CloseHandle(fileHandle), RES != 0);
}

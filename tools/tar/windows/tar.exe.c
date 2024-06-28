#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
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
#include "hc/tar.h"

#include "../common.c"

static void *outHandle;
static void *currentHandle;
static struct allocator alloc;

static int32_t init(hc_UNUSED char **envp) {
    GetSystemInfo(&systemInfo);

    return allocator_init(&alloc, (int64_t)1 << 32);
}

static void deinit(void) {
    allocator_deinit(&alloc);
}

static int32_t openOutput(char *name) {
    if (
        MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            name, -1,
            (uint16_t *)&buffer[0], sizeof(buffer) / sizeof(uint16_t)
        ) == 0
    ) return -1;

    outHandle = CreateFileW(
        (uint16_t *)&buffer[0],
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (outHandle == INVALID_HANDLE_VALUE) return -1;
    return 0;
}

static void closeOutput(void) {
    debug_CHECK(CloseHandle(outHandle), RES != 0);
}

static int32_t add(char *path) {
    struct state {
        struct state *prev;
        char **names;
        void *rootHandle;
    };

    struct state *state = NULL;
    int64_t allocSize = 0;

    if (
        MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            path, -1,
            (uint16_t *)&buffer[0], sizeof(buffer) / sizeof(uint16_t)
        ) <= 0
    ) return -1;
    currentHandle = CreateFileW(
        (uint16_t *)&buffer[0],
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (currentHandle == INVALID_HANDLE_VALUE) return -1;

    for (;;) {
        // Allocate new state.
        struct state *newState = &alloc.mem[allocSize];
        allocSize += sizeof(*newState);
        if (allocator_resize(&alloc, allocSize) < 0) goto cleanup_currentHandle;

        // Read directory entries.
        for (;;) {
            struct IO_STATUS_BLOCK ioStatusBlock;
            int32_t status = NtQueryDirectoryFile(
                currentHandle,
                NULL,
                NULL,
                NULL,
                &ioStatusBlock,
                &buffer[0],
                sizeof(buffer),
                FileNamesInformation,
                false,
                NULL,
                false
            );
            if (status == STATUS_NO_MORE_FILES) break;
            if (status < 0 || ioStatusBlock.information == 0) goto cleanup_currentHandle;

            struct FILE_NAMES_INFORMATION *current = (void *)&buffer[0];
            for (;;) {
                debug_ASSERT((allocSize & 7) == 0);
                int64_t *entrySize = &alloc.mem[allocSize];
                char *utf8 = (void *)&entrySize[1];
                int32_t utf16Count = current->fileNameLength / 2;
                int32_t maxUtf8Size = util_MAX_UTF8_PER_UTF16 * utf16Count;
                if (allocator_resize(&alloc, allocSize + (int64_t)sizeof(*entrySize) + maxUtf8Size + (int64_t)sizeof('\0')) < 0) goto cleanup_currentHandle;

                int32_t utf8Size = WideCharToMultiByte(
                    CP_UTF8,
                    WC_ERR_INVALID_CHARS,
                    (uint16_t *)&current[1], utf16Count,
                    utf8, maxUtf8Size,
                    NULL, NULL
                );
                if (utf8Size <= 0) goto cleanup_currentHandle;

                utf8[utf8Size++] = '\0';
                *entrySize = math_ALIGN_FORWARD((int32_t)sizeof(*entrySize) + utf8Size, 8);
                allocSize += *entrySize;

                if (current->nextEntryOffset == 0) break;
                current = (void *)current + current->nextEntryOffset;
            }
        }

        // Initialise and commit new state.
        newState->prev = state;
        newState->names = &alloc.mem[allocSize];
        newState->rootHandle = currentHandle;
        state = newState;

        // Build list of names.
        for (
            int64_t *current = (void *)&state[1];
            current != (void *)state->names;
            current = (void *)current + *current
        ) {
            char *currentName = (char *)&current[1];
            if (isDot(currentName) || isDotDot(currentName)) continue;
            allocSize += (int64_t)sizeof(state->names[0]);
            if (allocator_resize(&alloc, allocSize) < 0) goto cleanup_rootHandles;
            *((char **)&alloc.mem[allocSize] - 1) = currentName;
        }
        int64_t namesLength = (&alloc.mem[allocSize] - (void *)state->names) / (int64_t)sizeof(state->names[0]);
        sortNames(state->names, namesLength);

        // Loop over state entries.
        for (;;) {
            // Handle running out of entries.
            while (state->names == &alloc.mem[allocSize]) {
                debug_CHECK(CloseHandle(state->rootHandle), RES != 0);
                if (state->prev == NULL) return 0;

                // Pop state, but skip freeing memory.
                allocSize = (void *)state - &alloc.mem[0];
                state = state->prev;

                leaveDirectory();
            }

            // Handle next entry.
            char *name = state->names[0];
            ++state->names;

            // Open and write current name.
            int32_t nameUtf16Count = MultiByteToWideChar(
                CP_UTF8, MB_ERR_INVALID_CHARS,
                name, -1,
                (uint16_t *)&buffer[0], sizeof(buffer) / sizeof(uint16_t)
            );
            if (nameUtf16Count <= 0) goto cleanup_rootHandles;

            struct IO_STATUS_BLOCK ioStatusBlock;
            uint16_t nameUtf16Size = (uint16_t)(nameUtf16Count - 1) * sizeof(uint16_t); // Without null terminator.
            struct UNICODE_STRING nameUnicodeString = {
                .length = nameUtf16Size,
                .maximumLength = nameUtf16Size,
                .buffer = (uint16_t *)&buffer[0]
            };
            struct OBJECT_ATTRIBUTES objectAttributes = {
                .length = sizeof(objectAttributes),
                .rootDirectory = state->rootHandle,
                .objectName = &nameUnicodeString,
                .attributes = 0,
                .securityDescriptor = NULL,
                .securityQualityOfService = NULL
            };
            int32_t status = NtCreateFile(
                &currentHandle,
                GENERIC_READ | SYNCHRONIZE,
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
            if (status < 0) goto cleanup_rootHandles;

            int32_t nameLen = (int32_t)util_cstrLen(name);
            struct BY_HANDLE_FILE_INFORMATION handleInfo;
            if (GetFileInformationByHandle(currentHandle, &handleInfo) == 0) goto cleanup_currentHandle;
            int64_t fileSize;
            if (handleInfo.fileAttributes & FILE_ATTRIBUTE_DIRECTORY) fileSize = -1;
            else fileSize = ((int64_t)handleInfo.fileSizeHigh << 32) | (int64_t)handleInfo.fileSizeLow;

            if (
                writeRecord(
                    name, nameLen,
                    fileSize
                ) < 0
            ) goto cleanup_currentHandle;

            // If it was a directory, enter it. Otherwise, keep iterating entries.
            if (fileSize < 0) {
                debug_CHECK(enterDirectory(name, nameLen), RES == 0);
                break;
            } else debug_CHECK(CloseHandle(currentHandle), RES != 0);
        }
    }
    cleanup_currentHandle:
    debug_CHECK(CloseHandle(currentHandle), RES != 0);
    cleanup_rootHandles:
    while (state != NULL) {
        debug_CHECK(CloseHandle(state->rootHandle), RES != 0);
        state = state->prev;
    }
    return -1;
}

static int32_t readIntoBuffer(void) {
    uint32_t read;
    if (ReadFile(currentHandle, &buffer[0], sizeof(buffer), &read, NULL) == 0) return -1;
    return (int32_t)read;
}

static int32_t writeBuffer(int32_t size) {
    return util_writeAll(outHandle, &buffer[0], size);
}

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

#include "../common.c"

static void *outHandle;
static void *currentHandle;
static struct allocator alloc;

static int32_t init(hc_UNUSED char **envp, char *outFile) {
    GetSystemInfo(&systemInfo);

    if (
        MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            outFile, -1,
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

    return allocator_init(&alloc, (int64_t)1 << 32);
}

static void deinit(void) {
    debug_CHECK(CloseHandle(outHandle), RES != 0);
    allocator_deinit(&alloc);
}

static int32_t add(char *name) {
    struct state {
        struct state *prev;
        char **names;
        void *rootHandle;
    };

    // Prepare initial iteration.
    struct state *state = &alloc.mem[0];
    char *prefix = (void *)&state[1];
    int32_t prefixLen = 0;
    char **initialNames = (void *)&prefix[math_ALIGN_FORWARD(util_MAX_UTF8_PER_UTF16 * MAX_PATH, 8)];
    int64_t allocSize = (void *)&initialNames[1] - &alloc.mem[0];
    debug_ASSERT((allocSize & 7) == 0);
    if (allocator_resize(&alloc, allocSize) < 0) return -1;

    initialNames[0] = name;

    if (GetCurrentDirectoryW(sizeof(buffer) / sizeof(uint16_t), (uint16_t *)&buffer[0]) == 0) return -2;

    state->prev = NULL;
    state->names = initialNames;
    state->rootHandle = CreateFileW(
        (uint16_t *)&buffer[0],
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (state->rootHandle == INVALID_HANDLE_VALUE) return -3;

    for (;;) {
        name = state->names[0];
        ++state->names;

        // Open and write current name.
        int32_t nameUtf16Count = MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            name, -1,
            (uint16_t *)&buffer[0], sizeof(buffer) / sizeof(uint16_t)
        );
        if (nameUtf16Count <= 0) return -4;

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
        if (!NT_SUCCESS(status)) return -5;

        status = -1;
        int32_t nameLen = (int32_t)util_cstrLen(name);
        int64_t fileSize = 0; // Make static analysis happy.
        struct BY_HANDLE_FILE_INFORMATION handleInfo;
        if (GetFileInformationByHandle(currentHandle, &handleInfo) == 0) goto cleanup_currentHandle;
        if (handleInfo.fileAttributes & FILE_ATTRIBUTE_DIRECTORY) fileSize = -1;
        else fileSize = ((int64_t)handleInfo.fileSizeHigh << 32) | (int64_t)handleInfo.fileSizeLow;

        status = writeRecord(
            name, nameLen,
            prefix, prefixLen,
            fileSize
        );
        cleanup_currentHandle:
        if (status < 0) {
            debug_printNum("Failed to write record (", status, ")\n");
            debug_CHECK(CloseHandle(currentHandle), RES != 0);
            return -6;
        }

        // Push new state if directory.
        if (fileSize < 0) {
            struct state *prevState = state;
            state = &alloc.mem[allocSize];
            allocSize += sizeof(*state);
            if (allocator_resize(&alloc, allocSize) < 0) return -7;

            // Read directory entries.
            for (;;) {
                status = NtQueryDirectoryFile(
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
                if (!NT_SUCCESS(status) || ioStatusBlock.information == 0) return -8;

                struct FILE_NAMES_INFORMATION *current = (void *)&buffer[0];
                for (;;) {
                    debug_ASSERT((allocSize & 7) == 0);
                    int64_t *entrySize = &alloc.mem[allocSize];
                    char *utf8 = (void *)entrySize + sizeof(*entrySize);
                    int32_t utf16Count = current->fileNameLength / 2;
                    int32_t maxUtf8Size = util_MAX_UTF8_PER_UTF16 * utf16Count;
                    if (allocator_resize(&alloc, allocSize + (int64_t)sizeof(*entrySize) + maxUtf8Size + (int64_t)sizeof('\0')) < 0) return -9;

                    int32_t utf8Size = WideCharToMultiByte(
                        CP_UTF8,
                        WC_ERR_INVALID_CHARS,
                        (uint16_t *)&current[1], utf16Count,
                        utf8, maxUtf8Size,
                        NULL, NULL
                    );
                    if (utf8Size <= 0) return -10;

                    utf8[utf8Size++] = '\0';
                    *entrySize = math_ALIGN_FORWARD((int32_t)sizeof(*entrySize) + utf8Size, 8);
                    allocSize += *entrySize;

                    if (current->nextEntryOffset == 0) break;
                    current = (void *)current + current->nextEntryOffset;
                }
            }

            // Initialise new state.
            state->prev = prevState;
            state->names = &alloc.mem[allocSize];
            state->rootHandle = currentHandle;

            // Build list of names.
            for (
                int64_t *current = (void *)&state[1];
                current != (void *)state->names;
                current = (void *)current + *current
            ) {
                char *currentUtf8 = (char *)&current[1];
                // Skip `.` and `..`.
                if (
                    currentUtf8[0] == '.' && (
                        currentUtf8[1] == '\0' || (
                            currentUtf8[1] == '.' && currentUtf8[2] == '\0'
                        )
                    )
                ) continue;
                allocSize += (int64_t)sizeof(state->names[0]);
                if (allocator_resize(&alloc, allocSize) < 0) return -11;
                *((char **)&alloc.mem[allocSize] - 1) = currentUtf8;
            }
            int64_t namesLength = (&alloc.mem[allocSize] - (void *)state->names) / (int64_t)sizeof(state->names[0]);
            sortNames(state->names, namesLength);

            // Update prefix.
            if (prefixLen > 0) prefix[prefixLen++] = '/';
            hc_MEMCPY(&prefix[prefixLen], name, (uint64_t)nameLen);
            prefixLen += nameLen;
        } else debug_CHECK(CloseHandle(currentHandle), RES != 0);

        // Handle running out of entries.
        while (state->names == &alloc.mem[allocSize]) {
            if (state->prev == NULL) return 0;

            debug_CHECK(CloseHandle(state->rootHandle), RES != 0);

            // Pop state, but skip freeing memory.
            allocSize = (void *)state - &alloc.mem[0];
            state = state->prev;

            // Update prefix.
            while (prefixLen > 0) {
                --prefixLen;
                if (prefix[prefixLen] == '/') break;
            }
        }
    }
}

static int32_t readIntoBuffer(void) {
    uint32_t read;
    if (ReadFile(currentHandle, &buffer[0], sizeof(buffer), &read, NULL) == 0) return -1;
    return (int32_t)read;
}

static int32_t writeBuffer(int32_t size) {
    return util_writeAll(outHandle, &buffer[0], size);
}

#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#include "hc/freebsd/debug.c"
#include "hc/freebsd/heap.c"
#include "hc/freebsd/util.c"
#include "hc/freebsd/_start.c"
static int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"

#include "../common.c"

static int32_t outFd;
static int32_t currentFd;
static struct allocator alloc;

#define ALLOC_RESERVE_SIZE ((int64_t)1 << 32)

static int32_t init(hc_UNUSED char **envp, char *outFile) {
    debug_CHECK(elf_aux_info(AT_PAGESZ, &pageSize, sizeof(pageSize)), RES == 0);

    outFd = openat(AT_FDCWD, outFile, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0664);
    if (outFd < 0) return -1;

    return allocator_init(&alloc, ALLOC_RESERVE_SIZE);
}

static void deinit(void) {
    debug_CHECK(close(outFd), RES == 0);
    allocator_deinit(&alloc);
}

static int32_t add(char *name) {
    struct state {
        struct state *prev;
        char **names;
        int32_t rootFd;
        int32_t __pad;
    };

    // Prepare initial iteration.
    struct state *state = &alloc.mem[0];
    char *prefix = (void *)&state[1];
    int32_t prefixLen = 0;
    char **initialNames = (void *)&prefix[PATH_MAX];
    int64_t allocSize = (void *)&initialNames[1] - &alloc.mem[0];
    debug_ASSERT((allocSize & 7) == 0);
    if (allocator_resize(&alloc, allocSize) < 0) return -1;

    initialNames[0] = name;

    state->prev = NULL;
    state->names = initialNames;
    state->rootFd = AT_FDCWD;

    for (;;) {
        name = state->names[0];
        ++state->names;

        // Open and write current name.
        currentFd = openat(state->rootFd, name, O_RDONLY, 0);
        if (currentFd < 0) return -2;

        int32_t status = -1;
        int32_t nameLen = (int32_t)util_cstrLen(name);
        struct stat stat;
        if (fstatat(currentFd, "", &stat, AT_EMPTY_PATH) < 0) goto cleanup_currentFd;

        if (S_ISDIR(stat.st_mode)) stat.st_size = -1;
        else if (!S_ISREG(stat.st_mode)) goto cleanup_currentFd;

        status = writeRecord(
            name, nameLen,
            prefix, prefixLen,
            stat.st_size
        );
        cleanup_currentFd:
        if (status < 0) {
            debug_printNum("Failed to write record (", status, ")\n");
            debug_CHECK(close(currentFd), RES == 0);
            return -3;
        }

        // Push new state if directory.
        if (stat.st_size < 0) {
            struct state *prevState = state;
            state = &alloc.mem[allocSize];
            allocSize += sizeof(*state);

            // Read directory entries.
            for (;;) {
                debug_ASSERT((allocSize & 7) == 0);
                if (allocator_resize(&alloc, allocSize + 8192) < 0) return -4;

                int64_t numRead = getdents(currentFd, &alloc.mem[allocSize], alloc.size - allocSize);
                if (numRead <= 0) {
                    if (numRead == 0) break;
                    return -5;
                }
                allocSize += numRead;
            }
            allocSize = math_ALIGN_FORWARD(allocSize, 8);

            // Initialise new state.
            state->prev = prevState;
            state->names = &alloc.mem[allocSize];
            state->rootFd = currentFd;

            // Build list of names.
            for (
                struct dirent *current = (void *)&state[1];
                current != (void *)state->names;
                current = (void *)current + current->d_reclen
            ) {
                char *currentName = (void *)&current[1];
                // Skip `.` and `..`.
                if (
                    currentName[0] == '.' && (
                        currentName[1] == '\0' || (
                            currentName[1] == '.' && currentName[2] == '\0'
                        )
                    )
                ) continue;
                allocSize += (int64_t)sizeof(state->names[0]);
                if (allocator_resize(&alloc, allocSize) < 0) return -6;
                *((char **)&alloc.mem[allocSize] - 1) = currentName;
            }
            int64_t namesLength = (&alloc.mem[allocSize] - (void *)state->names) / (int64_t)sizeof(state->names[0]);
            sortNames(state->names, namesLength);

            // Update prefix.
            if (prefixLen > 0) prefix[prefixLen++] = '/';
            hc_MEMCPY(&prefix[prefixLen], name, (uint64_t)nameLen);
            prefixLen += nameLen;
        } else debug_CHECK(close(currentFd), RES == 0);

        // Handle running out of entries.
        while (state->names == &alloc.mem[allocSize]) {
            if (state->prev == NULL) return 0;

            debug_CHECK(close(state->rootFd), RES == 0);

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
    return (int32_t)read(currentFd, &buffer[0], sizeof(buffer));
}

static int32_t writeBuffer(int32_t size) {
    return util_writeAll(outFd, &buffer[0], size);
}

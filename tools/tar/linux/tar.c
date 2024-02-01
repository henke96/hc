#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/heap.c"
#include "hc/linux/helpers/_start.c"
static int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"

#include "../common.c"

static int32_t outFd;
static int32_t currentFd;
static struct allocator alloc;

static int32_t init(char **envp, char *outFile) {
    pageSize = util_getPageSize(util_getAuxv(envp));

    outFd = sys_openat(AT_FDCWD, outFile, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (outFd < 0) return -1;

    return allocator_init(&alloc, (int64_t)1 << 32);
}

static void deinit(void) {
    debug_CHECK(sys_close(outFd), RES == 0);
    allocator_deinit(&alloc);
}

static int32_t add(char *name) {
    int64_t nameSize = util_cstrLen(name) + 1;
    if (nameSize > NAME_MAX + 1) return -1;

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
    if (allocator_resize(&alloc, allocSize) < 0) return -2;

    initialNames[0] = name;

    state->prev = NULL;
    state->names = initialNames;
    state->rootFd = AT_FDCWD;

    for (;;) {
        name = state->names[0];
        ++state->names;

        // Open and write current name.
        currentFd = sys_openat(state->rootFd, name, O_RDONLY, 0);
        if (currentFd < 0) return -3;

        int32_t status = -1;
        int32_t nameLen = (int32_t)util_cstrLen(name);
        struct statx statx;
        statx.stx_mode = 0; // Make static analysis happy.
        if (sys_statx(currentFd, "", AT_EMPTY_PATH, STATX_TYPE | STATX_SIZE, &statx) < 0) goto cleanup_currentFd;

        if (S_ISDIR(statx.stx_mode)) statx.stx_size = -1;
        else if (!S_ISREG(statx.stx_mode)) goto cleanup_currentFd;

        status = writeRecord(
            name, nameLen,
            prefix, prefixLen,
            statx.stx_size
        );
        cleanup_currentFd:
        if (status < 0) {
            debug_printNum("Failed to write record (", status, ")\n");
            debug_CHECK(sys_close(currentFd), RES == 0);
            return -4;
        }

        // Push new state if directory.
        if (statx.stx_size < 0) {
            struct state *prevState = state;
            state = &alloc.mem[allocSize];
            allocSize += sizeof(*state);

            // Read directory entries.
            for (;;) {
                debug_ASSERT((allocSize & 7) == 0);
                if (allocator_resize(&alloc, allocSize + 8192) < 0) return -6;

                int64_t numRead = sys_getdents64(currentFd, &alloc.mem[allocSize], alloc.size - allocSize);
                if (numRead <= 0) {
                    if (numRead == 0) break;
                    return -7;
                }
                allocSize += numRead;
            }

            // Initialise new state.
            state->prev = prevState;
            state->names = &alloc.mem[allocSize];
            state->rootFd = currentFd;

            // Build list of names.
            for (
                struct linux_dirent64 *current = (void *)&state[1];
                current != (void *)state->names;
                current = (void *)current + current->d_reclen
            ) {
                // Skip `.` and `..`.
                if (
                    current->d_name[0] == '.' && (
                        current->d_name[1] == '\0' || (
                            current->d_name[1] == '.' && current->d_name[2] == '\0'
                        )
                    )
                ) continue;
                allocSize += (int64_t)sizeof(state->names[0]);
                if (allocator_resize(&alloc, allocSize) < 0) return -7;
                *((char **)&alloc.mem[allocSize] - 1) = &current->d_name[0];
            }
            int64_t namesLength = (&alloc.mem[allocSize] - (void *)state->names) / (int64_t)sizeof(state->names[0]);
            sortNames(state->names, namesLength);

            // Update prefix.
            if (prefixLen > 0) prefix[prefixLen++] = '/';
            hc_MEMCPY(&prefix[prefixLen], name, (uint64_t)nameLen);
            prefixLen += nameLen;
        } else debug_CHECK(sys_close(currentFd), RES == 0);

        // Handle running out of entries.
        while (state->names == &alloc.mem[allocSize]) {
            if (state->prev == NULL) return 0;

            debug_CHECK(sys_close(state->rootFd), RES == 0);

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
    return (int32_t)sys_read(currentFd, &buffer[0], sizeof(buffer));
}

static int32_t writeBuffer(int32_t size) {
    return util_writeAll(outFd, &buffer[0], size);
}

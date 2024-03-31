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

static int32_t init(char **envp) {
    pageSize = util_getPageSize(util_getAuxv(envp));

    return allocator_init(&alloc, (int64_t)1 << 32);
}

static void deinit(void) {
    allocator_deinit(&alloc);
}

static int32_t openOutput(char *name) {
    outFd = sys_openat(AT_FDCWD, name, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0664);
    return outFd;
}

static void closeOutput(void) {
    debug_CHECK(sys_close(outFd), RES == 0);
}

static int32_t add(char *name, char *root) {
    struct state {
        struct state *prev;
        char **names;
        int32_t rootFd;
        int32_t __pad;
    };

    // Prepare initial iteration.
    struct state *state = &alloc.mem[0];
    char **initialNames = (void *)&state[1];
    int64_t allocSize = (void *)&initialNames[1] - &alloc.mem[0];
    debug_ASSERT((allocSize & 7) == 0);
    if (allocator_resize(&alloc, allocSize) < 0) return -1;

    initialNames[0] = name;

    state->prev = NULL;
    state->names = initialNames;
    state->rootFd = sys_openat(AT_FDCWD, root, O_RDONLY, 0);
    if (state->rootFd < 0) return -2;

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
            // No need to resize allocator here since we do it unconditionally below.

            // Read directory entries.
            for (;;) {
                debug_ASSERT((allocSize & 7) == 0);
                if (allocator_resize(&alloc, allocSize + 8192) < 0) return -5;

                int64_t numRead = sys_getdents64(currentFd, &alloc.mem[allocSize], alloc.size - allocSize);
                if (numRead <= 0) {
                    if (numRead == 0) break;
                    return -6;
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
                char *currentName = &current->d_name[0];
                if (isDot(currentName) || isDotDot(currentName)) continue;
                allocSize += (int64_t)sizeof(state->names[0]);
                if (allocator_resize(&alloc, allocSize) < 0) return -7;
                *((char **)&alloc.mem[allocSize] - 1) = currentName;
            }
            int64_t namesLength = (&alloc.mem[allocSize] - (void *)state->names) / (int64_t)sizeof(state->names[0]);
            sortNames(state->names, namesLength);
        } else debug_CHECK(sys_close(currentFd), RES == 0);

        // Handle running out of entries.
        while (state->names == &alloc.mem[allocSize]) {
            debug_CHECK(sys_close(state->rootFd), RES == 0);
            if (state->prev == NULL) return 0;

            // Pop state, but skip freeing memory.
            allocSize = (void *)state - &alloc.mem[0];
            state = state->prev;

            leaveDirectory(state->names[-1]);
        }
    }
}

static int32_t readIntoBuffer(void) {
    return (int32_t)sys_read(currentFd, &buffer[0], sizeof(buffer));
}

static int32_t writeBuffer(int32_t size) {
    return util_writeAll(outFd, &buffer[0], size);
}

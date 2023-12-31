#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
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

#define ALLOC_RESERVE_SIZE ((int64_t)1 << 32)

static int32_t init(char **envp, char *outFile) {
    pageSize = util_getPageSize(util_getAuxv(envp));

    outFd = sys_openat(AT_FDCWD, outFile, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (outFd < 0) return -1;

    return allocator_init(&alloc, ALLOC_RESERVE_SIZE);
}

static void deinit(void) {
    debug_CHECK(sys_close(outFd), RES == 0);
    allocator_deinit(&alloc, ALLOC_RESERVE_SIZE);
}

static int32_t add(char *name) {
    int64_t nameSize = util_cstrLen(name) + 1;
    if (nameSize > INT16_MAX) return -1;

    #define STATE_SIZE 4096
    struct state {
        struct linux_dirent64 *current;
        struct linux_dirent64 *end;
        int32_t rootFd;
        int32_t __pad;
    };

    // Prepare initial iteration.
    if (allocator_resize(&alloc, STATE_SIZE) < 0) return -2;

    struct state *state = &alloc.mem[0];
    struct linux_dirent64 *initial = (void *)&state[1];

    initial->d_reclen = sizeof(*initial) + (uint16_t)nameSize;
    hc_MEMCPY(&initial[1], name, (uint16_t)nameSize);

    state->current = initial;
    state->end = (void *)initial + initial->d_reclen;
    state->rootFd = AT_FDCWD;

    char *prefix = (void *)state->end;
    int32_t prefixLen = 0;

    for (;;) {
        name = (char *)&state->current[1];
        state->current = (void *)state->current + state->current->d_reclen;

        // Open and write current name, unless it's `.` or `..`.
        if (name[0] != '.' || (name[1] != '\0' && (name[1] != '.' || name[2] != '\0'))) {
            currentFd = sys_openat(state->rootFd, name, O_RDONLY, 0);
            if (currentFd < 0) return -3;

            int32_t status = -1;
            int32_t nameLen = (int32_t)util_cstrLen(name);
            struct statx statx;
            statx.stx_mode = 0; // Make static analysis happy.
            if (sys_statx(currentFd, "", AT_EMPTY_PATH, STATX_TYPE | STATX_SIZE, &statx) < 0) goto cleanup_currentFd;

            if (statx.stx_mode & S_IFDIR) statx.stx_size = -1;
            if ((statx.stx_mode & (S_IFDIR | S_IFREG)) == 0) goto cleanup_currentFd;

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
                state = &alloc.mem[alloc.size];
                if (allocator_resize(&alloc, alloc.size + STATE_SIZE) < 0) return -5;

                state->current = NULL;
                state->end = NULL;
                state->rootFd = currentFd;

                // Update prefix.
                if (prefixLen > 0) prefix[prefixLen++] = '/';
                hc_MEMCPY(&prefix[prefixLen], name, (uint64_t)nameLen);
                prefixLen += nameLen; // `writeRecord` makes sure `nameLen` is max 100.
            } else debug_CHECK(sys_close(currentFd), RES == 0);
        }

        // Handle running out of cached entries.
        while (state->current == state->end) {
            if (alloc.size == STATE_SIZE) return 0; // Done with root state.

            state->current = (void *)&state[1];
            int64_t numRead = sys_getdents64(state->rootFd, state->current, STATE_SIZE - sizeof(*state));
            if (numRead <= 0) {
                debug_CHECK(sys_close(state->rootFd), RES == 0);
                if (numRead < 0) return -6;

                // We read the last entry, pop state.
                allocator_resize(&alloc, alloc.size - STATE_SIZE);
                state = &alloc.mem[alloc.size - STATE_SIZE];

                // Update prefix.
                while (prefixLen > 0) {
                    --prefixLen;
                    if (prefix[prefixLen] == '/') break;
                }
            } else {
                // We managed to read more entries, continue.
                state->end = (void *)state->current + numRead;
                break;
            }
        }
    }
}

static int32_t readIntoBuffer(void) {
    return (int32_t)sys_read(currentFd, &buffer[0], sizeof(buffer));
}

static int32_t write(int32_t size) {
    if (sys_write(outFd, &buffer[0], size) != size) return -1;
    return 0;
}

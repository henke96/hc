#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/base64.c"
#include "hc/debug.h"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/heap.c"

int32_t pageSize;
#define allocator_PAGE_SIZE pageSize
#include "hc/allocator.c"
#include "hc/linux/helpers/_start.c"

#include "../common.c"

static void initialise(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    pageSize = util_getPageSize(util_getAuxv(envp));
}

static int32_t changeDir(char *path) {
    return sys_chdir(path);
}

static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceLen, char *path, int32_t pathLen, bool asBase64) {
    // Add null terminator to path.
    char *pathZ = &alloc.mem[bufferLen];
    if (allocator_resize(&alloc, bufferLen + (int64_t)pathLen + 1) < 0) return -1;
    hc_MEMCPY(pathZ, path, (uint64_t)pathLen);
    pathZ[pathLen] = '\0';

    // Open file and get size.
    int32_t pathFd = sys_openat(AT_FDCWD, pathZ, O_RDONLY, 0);
    if (pathFd < 0) return -2;

    int32_t status;

    struct statx contentStat;
    contentStat.stx_size = 0;
    if (sys_statx(pathFd, "", AT_EMPTY_PATH, STATX_SIZE, &contentStat) < 0) {
        status = -3;
        goto cleanup_pathFd;
    }
    int64_t contentLen = (int64_t)contentStat.stx_size;

    // Resize buffer.
    int64_t insertLen = contentLen;
    if (asBase64) insertLen = base64_ENCODE_SIZE(contentLen);

    int64_t newBufferLen = bufferLen + (insertLen - replaceLen);
    if (allocator_resize(&alloc, newBufferLen) < 0) {
        status = -4;
        goto cleanup_pathFd;
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
        int64_t numRead = sys_read(pathFd, &content[index], remaining);
        if (numRead < 0) {
            if (numRead == -EINTR) continue;
            status = -5;
            goto cleanup_pathFd;
        }
        remaining -= numRead;
    }
    // Verify we are at end of file.
    char eofTest;
    if (sys_read(pathFd, &eofTest, 1) != 0) {
        status = -6;
        goto cleanup_pathFd;
    }

    // Convert to base 64 if requested. Can be done in place as we put content at end of insert gap.
    if (asBase64) base64_encode(&alloc.mem[replaceIndex], &content[0], contentLen);

    status = 0;
    cleanup_pathFd:
    debug_CHECK(sys_close(pathFd), RES == 0);
    return status;
}

static int32_t writeToFile(char *path, char *content, int64_t contentLen) {
    int32_t fd = sys_openat(AT_FDCWD, path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd < 0) return -1;

    int32_t status;
    int64_t remaining = contentLen;
    while (remaining > 0) {
        int64_t index = contentLen - remaining;
        int64_t written = sys_write(fd, &content[index], remaining);
        if (written < 0) {
            if (written == -EINTR) continue;
            status = -2;
            goto cleanup_fd;
        }
        remaining -= written;
    }

    status = 0;
    cleanup_fd:
    debug_CHECK(sys_close(fd), RES == 0);
    return status;
}

static void deinit(void) {
    for (int32_t *includePathFd = alloc.mem; (char *)includePathFd != buffer; ++includePathFd) {
        debug_CHECK(close(*includePathFd), RES == 0);
    }
}

static int32_t init(char **includePaths) {
    int64_t i = 0;
    int32_t *includeFds = alloc.mem;
    for (; includePaths[i] != NULL; ++i) {
        if (allocator_resize(&alloc, (i + 1) * (int64_t)sizeof(int32_t)) < 0) break;

        int32_t fd = openat(AT_FDCWD, includePaths[i], O_RDONLY, 0);
        if (fd < 0) break;
        includeFds[i] = fd;
    }
    buffer = (void *)&includeFds[i];
    if (includePaths[i] != NULL) {
        deinit();
        return -1;
    }
    return 0;
}

static int32_t replaceWithFile(int64_t replaceIndex, int64_t replaceSize, char *path, int32_t pathLen, bool asBase64) {
    // Add null terminator to path.
    char *pathZ = &alloc.mem[alloc.size];
    if (allocator_resize(&alloc, alloc.size + (int64_t)pathLen + 1) < 0) return -1;
    hc_MEMCPY(pathZ, path, (uint64_t)pathLen);
    pathZ[pathLen] = '\0';

    // Find and open path relative to include paths.
    int32_t pathFd;
    for (int32_t *includePathFd = alloc.mem; (char *)includePathFd != buffer; ++includePathFd) {
        pathFd = openat(*includePathFd, pathZ, O_RDONLY, 0);
        if (pathFd >= 0) goto foundPath;
    }
    return -2;
    foundPath:;

    // Get file size.
    int32_t status;
    struct stat contentStat;
    if (fstatat(pathFd, "", &contentStat, AT_EMPTY_PATH) < 0) {
        status = -3;
        goto cleanup_pathFd;
    }
    int64_t contentSize = contentStat.st_size;

    // Resize buffer.
    int64_t insertSize = contentSize;
    if (asBase64) insertSize = base64_ENCODE_SIZE(contentSize);

    int64_t newBufferSize = bufferSize + (insertSize - replaceSize);
    // Allocate an extra byte to be able to verify EOF when reading.
    if (allocator_resize(&alloc, &buffer[newBufferSize + 1] - (char *)alloc.mem) < 0) {
        status = -4;
        goto cleanup_pathFd;
    }

    // Move existing content to make room.
    hc_MEMMOVE(
        &buffer[replaceIndex + insertSize],
        &buffer[replaceIndex + replaceSize],
        (uint64_t)(bufferSize - (replaceSize + replaceIndex))
    );
    bufferSize = newBufferSize;

    // Read content into buffer, and verify EOF by attempting to read an extra byte.
    char *content = &buffer[replaceIndex + insertSize - contentSize];
    if (util_readAll(pathFd, content, contentSize + 1) != contentSize) {
        status = -5;
        goto cleanup_pathFd;
    }

    // Convert to base 64 if requested. Can be done in place as we put content at end of insert gap.
    if (asBase64) base64_encode(&buffer[replaceIndex], content, contentSize);

    status = 0;
    cleanup_pathFd:
    debug_CHECK(close(pathFd), RES == 0);
    return status;
}

static int32_t writeToFile(char *path, char *content, int64_t contentSize) {
    int32_t fd = openat(AT_FDCWD, path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd < 0) return -1;

    int32_t status = util_writeAll(fd, content, contentSize);
    debug_CHECK(close(fd), RES == 0);
    return status;
}

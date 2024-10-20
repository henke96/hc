static int32_t outFd;
static int32_t currentFd;
static struct allocator alloc;

static int32_t init(char **envp) {
    initPageSize(envp);

    return allocator_init(&alloc, (int64_t)1 << 32);
}

static void deinit(void) {
    allocator_deinit(&alloc);
}

static int32_t openOutput(char *name) {
    outFd = openat(AT_FDCWD, name, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0664);
    return outFd;
}

static void closeOutput(void) {
    debug_CHECK(close(outFd), RES == 0);
}

static int32_t add(char *path) {
    struct state {
        struct state *prev;
        char **names;
        int32_t rootFd;
        int32_t __pad;
    };

    struct state *state = NULL;
    int64_t allocSize = 0;
    currentFd = openat(AT_FDCWD, path, O_RDONLY, 0);
    if (currentFd < 0) return -1;

    for (;;) {
        // Allocate new state.
        struct state *newState = &alloc.mem[allocSize];
        allocSize += sizeof(*newState);
        // `allocator_resize` is called unconditionally below.

        // Read directory entries.
        for (;;) {
            debug_ASSERT((allocSize & 7) == 0);
            if (allocator_resize(&alloc, allocSize + 8192) < 0) goto cleanup_currentFd;

            int64_t numRead = getdents(currentFd, &alloc.mem[allocSize], alloc.size - allocSize);
            if (numRead <= 0) {
                if (numRead == 0) break;
                goto cleanup_currentFd;
            }
            allocSize += numRead;
        }

        // Initialise and commit new state.
        newState->prev = state;
        newState->names = &alloc.mem[allocSize];
        newState->rootFd = currentFd;
        state = newState;

        // Build list of names.
        for (
            struct dirent *current = (void *)&state[1];
            current != (void *)state->names;
            current = (void *)current + current->d_reclen
        ) {
            char *currentName = ix_D_NAME(current);
            if (isDot(currentName) || isDotDot(currentName)) continue;
            allocSize += (int64_t)sizeof(state->names[0]);
            if (allocator_resize(&alloc, allocSize) < 0) goto cleanup_rootFds;
            *((char **)&alloc.mem[allocSize] - 1) = currentName;
        }
        int64_t namesLength = (&alloc.mem[allocSize] - (void *)state->names) / (int64_t)sizeof(state->names[0]);
        sortNames(state->names, namesLength);

        // Loop over state entries.
        for (;;) {
            // Handle running out of entries.
            while (state->names == &alloc.mem[allocSize]) {
                debug_CHECK(close(state->rootFd), RES == 0);
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
            currentFd = openat(state->rootFd, name, O_RDONLY, 0);
            if (currentFd < 0) goto cleanup_rootFds;

            int32_t nameLen = (int32_t)util_cstrLen(name);
            struct stat stat;
            stat.st_mode = 0; // Make static analysis happy.
            if (fstatat(currentFd, "", &stat, AT_EMPTY_PATH) < 0) goto cleanup_currentFd;

            if (S_ISDIR(stat.st_mode)) stat.st_size = -1;
            else if (!S_ISREG(stat.st_mode)) goto cleanup_currentFd;

            if (
                writeRecord(
                    name, nameLen,
                    stat.st_size
                ) < 0
            ) goto cleanup_currentFd;

            // If it was a directory, enter it. Otherwise, keep iterating entries.
            if (stat.st_size < 0) {
                debug_CHECK(enterDirectory(name, nameLen), RES == 0);
                break;
            } else debug_CHECK(close(currentFd), RES == 0);
        }
    }
    cleanup_currentFd:
    debug_CHECK(close(currentFd), RES == 0);
    cleanup_rootFds:
    while (state != NULL) {
        debug_CHECK(close(state->rootFd), RES == 0);
        state = state->prev;
    }
    return -1;
}

static int32_t readIntoBuffer(void) {
    return (int32_t)read(currentFd, &buffer[0], sizeof(buffer));
}

static int32_t writeBuffer(int32_t size) {
    return util_writeAll(outFd, &buffer[0], size);
}

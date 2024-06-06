static int32_t openInput(char *path);
static int32_t readInput(void);
static void closeInput(void);

static int32_t createDir(char *prefix, char *name);
static int32_t createFile(char *prefix, char *name);
static int32_t writeToFile(int32_t size);
static void closeFile(void);

static char buffer[tar_RECORD_SIZE] hc_ALIGNED(16);

static int64_t parseSize(void) {
    int64_t size = 0;
    for (int32_t i = 0; i < 11; ++i) {
        uint32_t digit = (uint32_t)buffer[tar_OFFSET_SIZE + i] - '0';
        if (digit > 7) return -1;
        size <<= 3;
        size += digit;
    }
    return size;
}

int32_t start(int32_t argc, char **argv, hc_UNUSED char **envp) {
    if (argc != 2) {
        debug_print("Usage: untar ARCHIVE\n");
        return 1;
    }

    int32_t status = openInput(argv[1]);
    if (status < 0) {
        debug_print("Failed to open input\n");
        return 1;
    }

    status = 1;
    int32_t zeroRecordCount = 0;
    for (;;) {
        if (readInput() < 0) break;

        char typeflag = buffer[tar_OFFSET_TYPEFLAG];
        // Check for 2 zero records, indicating end of archive.
        if (typeflag == 0) {
            for (int32_t i = 0; i < 512; ++i) {
                if (buffer[i] != 0) goto notZero;
            }
            if (++zeroRecordCount < 2) continue;
            status = 0;
            notZero:
            break;
        }
        if (zeroRecordCount > 0) break;

        // Handle directories.
        if (typeflag == '5') {
            if (createDir(&buffer[tar_OFFSET_PREFIX], &buffer[tar_OFFSET_NAME]) < 0) break;
            continue;
        }
        if (typeflag != '0') break;

        // Handle files.
        int64_t size = parseSize();
        if (size < 0) break;
        if (createFile(&buffer[tar_OFFSET_PREFIX], &buffer[tar_OFFSET_NAME]) < 0) break;

        for (int64_t written = 0;; written += tar_RECORD_SIZE) {
            if (readInput() < 0) break;
            int64_t remaining = size - written;
            if (remaining <= tar_RECORD_SIZE) {
                if (writeToFile((int32_t)remaining) == 0) status = 0;
                break;
            }
            if (writeToFile(tar_RECORD_SIZE) < 0) break;
        }

        closeFile();
        if (status != 0) break;
    }

    closeInput();
    return status;
}

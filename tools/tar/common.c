static int32_t init(char **envp, char *outFile);
static void deinit(void);
static int32_t readIntoBuffer(void);
static int32_t write(int32_t size);
static int32_t add(char *name);

static char buffer[65536] hc_ALIGNED(16);

#define NAME 0
#define MODE 100
#define SIZE 124
#define MTIME 136
#define CHKSUM 148
#define TYPEFLAG 156
#define MAGIC 257
#define VERSION 263
#define PREFIX 345
#define END 512

static char octalTable[8] = "01234567";

static int32_t writeRecord(char *name, int32_t nameLen, char *prefix, int32_t prefixLen, int64_t fileSize) {
    if (nameLen > 100 || prefixLen > 155 || fileSize > 077777777777) return -1;

    char typeflag = '0';
    if (fileSize < 0) { // Directory.
        typeflag = '5';
        fileSize = 0;
    }

    // Fill in name and prefix, zero rest.
    hc_MEMCPY(&buffer[NAME], name, (uint64_t)nameLen);
    hc_MEMSET(&buffer[NAME + nameLen], 0, (uint64_t)(PREFIX - NAME - nameLen));
    hc_MEMCPY(&buffer[PREFIX], prefix, (uint64_t)prefixLen);
    hc_MEMSET(&buffer[PREFIX + prefixLen], 0, (uint64_t)(END - PREFIX - prefixLen));

    // Fill in other fields.
    hc_MEMCPY(&buffer[MODE], hc_STR_COMMA_LEN("0000777"));
    for (int32_t i = 0; i < 11; ++i) {
        buffer[SIZE + 10 - i] = octalTable[(fileSize >> 3 * i) & 7];
    }
    hc_MEMCPY(&buffer[MTIME], hc_STR_COMMA_LEN("00000000000"));
    hc_MEMCPY(&buffer[CHKSUM], hc_STR_COMMA_LEN("        "));
    buffer[TYPEFLAG] = typeflag;
    hc_MEMCPY(&buffer[MAGIC], hc_STR_COMMA_LEN("ustar"));
    hc_MEMCPY(&buffer[VERSION], hc_STR_COMMA_LEN("00"));

    // Calculate checksum.
    int32_t checksum = 0;
    for (int32_t i = 0; i < END; ++i) {
        checksum += buffer[i];
    }
    for (int32_t i = 0; i < 7; ++i) {
        buffer[CHKSUM + 6 - i] = octalTable[(checksum >> 3 * i) & 7];
    }

    // Write header.
    if (write(END) < 0) return -2;
    if (fileSize == 0) return 0;

    // Write file content.
    int64_t totalWritten = 0;
    for (;;) {
        int32_t numRead = readIntoBuffer();
        if (numRead <= 0) {
            if (numRead < 0) return -3;
            if (totalWritten != fileSize) return -4;
            break;
        }
        if (write(numRead) < 0) return -5;
        totalWritten += numRead;
    }
    int32_t numPadBytes = math_PAD_BYTES(totalWritten, 512);
    hc_MEMSET(&buffer[0], 0, (uint64_t)numPadBytes);
    if (write(numPadBytes) < 0) return -6;
    return 0;
}


int32_t start(int32_t argc, char **argv, char **envp) {
    if (argc < 2) return 1;

    if (init(envp, argv[1]) < 0) return 1;

    int32_t status;
    for (int32_t i = 2; i < argc; ++i) {
        status = add(argv[i]);
        if (status < 0) {
            debug_printNum("Failed to add entry (", status, ")\n");
            status = 1;
            goto cleanup;
        }
    }
    hc_MEMSET(&buffer[0], 0, 2 * END);
    status = (write(2 * END) < 0);
    cleanup:
    deinit();
    return status;
}

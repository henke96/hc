// ALIGN must be power of 2.
#define util_ALIGN_FORWARD(X, ALIGN) (((X) + ((ALIGN) - 1)) & ~((ALIGN) - 1))
#define util_PAD_BYTES(X, ALIGN) (-(X) & ((ALIGN) - 1))
// Check if [X_START, X_END) and [Y_START, Y_END) overlap.
#define util_RANGES_OVERLAP(X_START, X_END, Y_START, Y_END) ((X_START) < (Y_END) && (Y_START) < (X_END))

hc_UNUSED
static int64_t util_cstrLen(const char *cstring) {
    const char *c = cstring;
    for (; *c != '\0'; ++c);
    return c - cstring;
}

hc_UNUSED
static int32_t util_cstrCmp(const char *left, const char *right) {
    for (;;) {
        int32_t diff = *left - *right;
        if (diff != 0 || *left == '\0') return diff;
        ++left;
        ++right;
    }
}

hc_UNUSED
static void util_strToUtf16(uint16_t *dest, char *src, int64_t length) {
    for (int64_t i = 0; i < length; ++i) *dest++ = (uint16_t)*src++;
}

#define util_INT32_MAX_CHARS 11
#define util_UINT32_MAX_CHARS 10
#define util_INT64_MAX_CHARS 20
#define util_UINT64_MAX_CHARS 20
// Writes max 20 characters.
// `bufferEnd` points 1 past where last digit is written.
// Returns pointer to first character of result.
hc_UNUSED static char *util_intToStr(char *bufferEnd, int64_t number) {
    uint64_t n = number < 0 ? -((uint64_t)number) : (uint64_t)number;
    do {
        *--bufferEnd = (char)('0' + n % 10);
        n /= 10;
    } while (n != 0);

    if (number < 0) *--bufferEnd = '-';
    return bufferEnd;
}

// Writes max 20 characters.
// `bufferEnd` points 1 past where last digit is written.
// Returns pointer to first character of result.
hc_UNUSED static char *util_uintToStr(char *bufferEnd, uint64_t number) {
    do {
        *--bufferEnd = (char)('0' + number % 10);
        number /= 10;
    } while (number != 0);
    return bufferEnd;
}

// Expects `buffer` to have 1 or more digits followed by `terminator`.
// Returns the number of characters in the parsed number (0 on errors).
// Sets `*number` to the parsed value if successful.
hc_UNUSED static int32_t util_strToUint(const void *buffer, char terminator, uint64_t *number) {
    uint64_t result = 0;
    int32_t i = 0;
    do {
        uint64_t digitValue = (uint64_t)((uint8_t *)buffer)[i] - '0';
        if (
            (digitValue > 9) || (result > (UINT64_MAX - digitValue) / 10)
        ) return 0; // Not a digit or overflow.

        result = result * 10 + digitValue;
        ++i;
    } while (((char *)buffer)[i] != terminator);

    *number = result;
    return i;
}

// Expects `buffer` to have 1 or more digits followed by `terminator`, optionally starting with a minus sign.
// Returns the number of characters in the parsed number (0 on errors).
// Sets `*number` to the parsed value if successful.
hc_UNUSED static int32_t util_strToInt(const char *buffer, char terminator, int64_t *number) {
    if (buffer[0] == '-') {
        uint64_t value;
        int32_t parsed = util_strToUint(&buffer[1], terminator, &value);
        if (parsed == 0 || value > (uint64_t)INT64_MAX + 1) return 0;
        *number = (int64_t)-value;
        return 1 + parsed;
    } else {
        uint64_t value;
        int32_t parsed = util_strToUint(buffer, terminator, &value);
        if (parsed == 0 || value > (uint64_t)INT64_MAX) return 0;
        *number = (int64_t)value;
        return parsed;
    }
}

// Expects `buffer` to have 1 or more hex digits followed by `terminator`.
// Returns the number of characters in the parsed number (0 on errors).
// Sets `*number` to the parsed value if successful.
hc_UNUSED static int32_t util_hexToUint(const void *buffer, char terminator, uint64_t *number) {
    uint64_t result = 0;
    int32_t i = 0;
    do {
        uint64_t digitValue = ((uint8_t *)buffer)[i] - '0';
        if (digitValue > 9) {
            digitValue += (uint64_t)'0' - 'a';
            if (digitValue > 5) {
                digitValue += (uint64_t)'a' - 'A';
                if (digitValue > 5) return 0; // Not a hex digit.
            }
            digitValue += 10;
        }
        if (result > (UINT64_MAX >> 4)) return 0; // Overflow.

        result = (result << 4) | digitValue;
        ++i;
    } while (((char *)buffer)[i] != terminator);

    *number = result;
    return i;
}

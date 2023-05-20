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

// Returns number of characters in the parsed number (max `maxChars`), 0 if parsing failed, or -1 on overflow.
// The result is stored in `*number` if successful.
hc_UNUSED static int32_t util_strToUint(const void *buffer, int64_t maxChars, uint64_t *number) {
    uint64_t result = 0;
    int32_t i = 0;
    for (; i < maxChars; ++i) {
        uint64_t digitValue = (uint64_t)((const uint8_t *)buffer)[i] - '0';
        if (digitValue > 9) break;

        if (result > (UINT64_MAX - digitValue) / 10) return -1;
        result = result * 10 + digitValue;
    }

    if (i > 0) *number = result;
    return i;
}

// Returns number of characters in the parsed number (max `maxChars`), 0 if parsing failed, or -1 on overflow.
// The result is stored in `*number` if successful.
hc_UNUSED static int32_t util_strToInt(const void *buffer, int64_t maxChars, int64_t *number) {
    if (maxChars <= 0) return 0;

    uint64_t negative = ((const char *)buffer)[0] == '-';

    uint64_t maxNumber = (uint64_t)INT64_MAX + negative;
    uint64_t result = 0;
    int32_t i = (int32_t)negative;
    for (; i < maxChars; ++i) {
        uint64_t digitValue = (uint64_t)((const uint8_t *)buffer)[i] - '0';
        if (digitValue > 9) break;

        if (result > (maxNumber - digitValue) / 10) return -1;
        result = result * 10 + digitValue;
    }

    if (i > (int32_t)negative) {
        *number = (int64_t)(result * (1 - (negative << 1)));
        return i;
    }
    return 0;
}

// Returns number of characters in the parsed number (max `maxChars`), 0 if parsing failed, or -1 on overflow.
// The result is stored in `*number` if successful.
hc_UNUSED static int32_t util_hexToUint(const void *buffer, int64_t maxChars, uint64_t *number) {
    uint64_t result = 0;
    int32_t i = 0;
    for (; i < maxChars; ++i) {
        uint64_t digitValue = ((const uint8_t *)buffer)[i];
        digitValue -= '0';
        if (digitValue > 9) {
            digitValue += (uint64_t)'0' - 'A';
            if (digitValue > 5) {
                digitValue += (uint64_t)'A' - 'a';
                if (digitValue > 5) break; // Not a hex digit.
            }
            digitValue += 10;
        }
        if (result > (UINT64_MAX >> 4)) return -1; // Overflow.

        result = (result << 4) | digitValue;
    }

    if (i > 0) *number = result;
    return i;
}

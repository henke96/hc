void *memset(void *dest, int32_t c, size_t n) hc_NO_BUILTIN {
    uint64_t expanded = 0x0101010101010101 * (uint64_t)(c & 0xFF);
    while (n > 16) {
        n -= 16;
        hc_MEMCPY(dest + n, &expanded, 8);
        hc_MEMCPY(dest + n + 8, &expanded, 8);
    }
    if (n >= 8) {
        hc_MEMCPY(dest, &expanded, 8);
        hc_MEMCPY(dest + n - 8, &expanded, 8);
    } else if (n >= 4) {
        hc_MEMCPY(dest, &expanded, 4);
        hc_MEMCPY(dest + n - 4, &expanded, 4);
    } else if (n >= 2) {
        hc_MEMCPY(dest, &expanded, 2);
        hc_MEMCPY(dest + n - 2, &expanded, 2);
    } else if (n != 0) *(char *)dest = (char)expanded;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) hc_NO_BUILTIN {
    char *d = dest;
    const char *s = src;
    if (d < s) {
        for (; n != 0; --n) *d++ = *s++;
    } else {
        while (n) {
            --n;
            d[n] = s[n];
        }
    }
    return dest;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) hc_ALIAS("memmove");

int32_t memcmp(const void *left, const void *right, size_t n) hc_NO_BUILTIN {
    const char *l = left;
    const char *r = right;
    for (;;) {
        if (n == 0) return 0;
        int32_t diff = *l - *r;
        if (diff != 0) return diff;
        ++l;
        ++r;
        --n;
    }
}

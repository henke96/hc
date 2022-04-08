#if !hc_LIBC
// Symbols expected by gcc/clang in freestanding mode.
// Put them in separate sections to allow linker to discard unused ones.
hc_SECTION(".text.memset")
void *memset(void *dest, int32_t c, size_t n) {
    char *d = dest;
    for (; n != 0; --n) *d++ = (char)c;
    return dest;
}

hc_SECTION(".text.memmove")
void *memmove(void *dest, const void *src, size_t n) {
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

hc_SECTION(".text.memcpy")
void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    char *d = dest;
    const char *s = src;
    for (; n != 0; --n) *d++ = *s++;
    return dest;
}

hc_SECTION(".text.memcmp")
int32_t memcmp(const void *left, const void *right, size_t n) {
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
#endif
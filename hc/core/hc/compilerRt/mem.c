// TODO: riscv64 codegen looks terrible (avoids unaligned stores). Try `-Xclang -target-feature -Xclang +unaligned-scalar-mem` on real hw.

void *memset(void *dest, int32_t c, size_t n) hc_NO_BUILTIN {
    uint64_t expanded = 0x0101010101010101 * (uint64_t)(c & 0xFF);
    while (n >= 16) {
        n -= 16;
        hc_MEMCPY(dest + n, &expanded, 8);
        hc_MEMCPY(dest + n + 8, &expanded, 8);
    }
    if (n == 0) return dest;
    if (n >= 8) {
        hc_MEMCPY(dest, &expanded, 8);
        hc_MEMCPY(dest + n - 8, &expanded, 8);
    } else if (n >= 4) {
        hc_MEMCPY(dest, &expanded, 4);
        hc_MEMCPY(dest + n - 4, &expanded, 4);
    } else if (n >= 2) {
        hc_MEMCPY(dest, &expanded, 2);
        hc_MEMCPY(dest + n - 2, &expanded, 2);
    } else *(char *)dest = (char)expanded;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) hc_NO_BUILTIN {
    char *d = dest;
    const char *s = src;
    if (n >= 16) {
        uint128_t temp;
        if (d < s) {
            size_t iEnd = n & ~(size_t)15;
            size_t i = 0;
            do {
                hc_MEMCPY(&temp, s + i, 16);
                hc_MEMCPY(d + i, &temp, 16);
                i += 16;
            } while (i < iEnd);
            n &= 15;
            d += iEnd;
            s += iEnd;
        } else {
            do {
                n -= 16;
                hc_MEMCPY(&temp, s + n, 16);
                hc_MEMCPY(d + n, &temp, 16);
            } while (n >= 16);
        }
    }
    if (n == 0) return dest;
    if (n >= 8) {
        uint64_t temp1, temp2;
        hc_MEMCPY(&temp1, s, 8);
        hc_MEMCPY(&temp2, s + n - 8, 8);
        hc_MEMCPY(d, &temp1, 8);
        hc_MEMCPY(d + n - 8, &temp2, 8);
    } else if (n >= 4) {
        uint32_t temp1, temp2;
        hc_MEMCPY(&temp1, s, 4);
        hc_MEMCPY(&temp2, s + n - 4, 4);
        hc_MEMCPY(d, &temp1, 4);
        hc_MEMCPY(d + n - 4, &temp2, 4);
    } else if (n >= 2) {
        uint16_t temp1, temp2;
        hc_MEMCPY(&temp1, s, 2);
        hc_MEMCPY(&temp2, s + n - 2, 2);
        hc_MEMCPY(d, &temp1, 2);
        hc_MEMCPY(d + n - 2, &temp2, 2);
    } else *d = *s;
    return dest;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) hc_ALIAS("memmove");

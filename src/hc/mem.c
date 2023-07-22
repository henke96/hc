hc_UNUSED
static int32_t mem_compareConstantTime(const void *a, const void *b, int64_t size) hc_NO_BUILTIN {
    int32_t x = 0;
    const uint8_t *a8 = a;
    const uint8_t *b8 = b;
    for (int64_t i = 0; i < size; ++i) {
        x |= a8[i] ^ b8[i];
    }
    return x;
}

hc_UNUSED
static hc_INLINE uint16_t mem_loadU16(const void *in) {
    uint16_t x;
    hc_MEMCPY(&x, in, 2);
    return x;
}

hc_UNUSED
static hc_INLINE uint32_t mem_loadU32(const void *in) {
    uint32_t x;
    hc_MEMCPY(&x, in, 4);
    return x;
}

hc_UNUSED
static hc_INLINE uint64_t mem_loadU64(const void *in) {
    uint64_t x;
    hc_MEMCPY(&x, in, 8);
    return x;
}

hc_UNUSED
static hc_INLINE void mem_storeU16(void *out, uint16_t in) {
    hc_MEMCPY(out, &in, 2);
}

hc_UNUSED
static hc_INLINE void mem_storeU32(void *out, uint32_t in) {
    hc_MEMCPY(out, &in, 4);
}

hc_UNUSED
static hc_INLINE void mem_storeU64(void *out, uint64_t in) {
    hc_MEMCPY(out, &in, 8);
}

hc_UNUSED
static hc_INLINE uint16_t mem_loadU16BE(const void *in) {
    uint16_t x;
    hc_MEMCPY(&x, in, 2);
    return hc_BSWAP16(x);
}

hc_UNUSED
static hc_INLINE uint32_t mem_loadU32BE(const void *in) {
    uint32_t x;
    hc_MEMCPY(&x, in, 4);
    return hc_BSWAP32(x);
}

hc_UNUSED
static hc_INLINE uint64_t mem_loadU64BE(const void *in) {
    uint64_t x;
    hc_MEMCPY(&x, in, 8);
    return hc_BSWAP64(x);
}

hc_UNUSED
static hc_INLINE void mem_storeU16BE(void *out, uint16_t in) {
    in = hc_BSWAP16(in);
    hc_MEMCPY(out, &in, 2);
}

hc_UNUSED
static hc_INLINE void mem_storeU32BE(void *out, uint32_t in) {
    in = hc_BSWAP32(in);
    hc_MEMCPY(out, &in, 4);
}

hc_UNUSED
static hc_INLINE void mem_storeU64BE(void *out, uint64_t in) {
    in = hc_BSWAP64(in);
    hc_MEMCPY(out, &in, 8);
}

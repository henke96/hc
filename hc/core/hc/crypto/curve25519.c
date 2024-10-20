// Shared code for x25519 and ed25519.
// Public domain by Andrew M. <liquidsun@gmail.com> (curve25519-donna & ed25519-donna)

// Curve: y^2 = x^3 + 486662*x^2 + x
// p = 2^255 - 19

static const uint64_t curve25519_zero[5] = {0};

// Load 32 byte number from `in` into `a`.
hc_UNUSED static void curve25519_load(uint64_t *a, const void *in) {
    a[0] = mem_loadU64(in) & 0x7FFFFFFFFFFFF;
    a[1] = (mem_loadU64(in + 6) >> 3) & 0x7FFFFFFFFFFFF;
    a[2] = (mem_loadU64(in + 12) >> 6) & 0x7FFFFFFFFFFFF;
    a[3] = (mem_loadU64(in + 19) >> 1) & 0x7FFFFFFFFFFFF;
    a[4] = (mem_loadU64(in + 24) >> 12) & 0x7FFFFFFFFFFFF;
}

// Store fully reduced `a` as 32 byte number into `out`.
hc_UNUSED static void curve25519_store(void *out, uint64_t *a) {
    mem_storeU64(&out[0], a[0] | (a[1] << 51));
    mem_storeU64(&out[8], (a[1] >> 13) | (a[2] << 38));
    mem_storeU64(&out[16], (a[2] >> 26) | (a[3] << 25));
    mem_storeU64(&out[24], (a[3] >> 39) | (a[4] << 12));
}

// Fully reduce `a`.
hc_UNUSED static void curve25519_reduce(uint64_t *a) {
    a[1] += a[0] >> 51;
    a[0] &= 0x7FFFFFFFFFFFF;
    a[2] += a[1] >> 51;
    a[1] &= 0x7FFFFFFFFFFFF;
    a[3] += a[2] >> 51;
    a[2] &= 0x7FFFFFFFFFFFF;
    a[4] += a[3] >> 51;
    a[3] &= 0x7FFFFFFFFFFFF;
    a[0] += 19 * (a[4] >> 51);
    a[4] &= 0x7FFFFFFFFFFFF;

    a[1] += a[0] >> 51;
    a[0] &= 0x7FFFFFFFFFFFF;
    a[2] += a[1] >> 51;
    a[1] &= 0x7FFFFFFFFFFFF;
    a[3] += a[2] >> 51;
    a[2] &= 0x7FFFFFFFFFFFF;
    a[4] += a[3] >> 51;
    a[3] &= 0x7FFFFFFFFFFFF;
    a[0] += 19 * (a[4] >> 51);
    a[4] &= 0x7FFFFFFFFFFFF;

    // Now a is between 0 and 2^255-1, properly carried.
    // Case 1: between 0 and 2^255-20.
    // Case 2: between 2^255-19 and 2^255-1.

    a[0] += 19;

    a[1] += a[0] >> 51;
    a[0] &= 0x7FFFFFFFFFFFF;
    a[2] += a[1] >> 51;
    a[1] &= 0x7FFFFFFFFFFFF;
    a[3] += a[2] >> 51;
    a[2] &= 0x7FFFFFFFFFFFF;
    a[4] += a[3] >> 51;
    a[3] &= 0x7FFFFFFFFFFFF;
    a[0] += 19 * (a[4] >> 51);
    a[4] &= 0x7FFFFFFFFFFFF;

    // Now between 19 and 2^255-1 in both cases, and offset by 19.

    a[0] += 0x8000000000000 - 19;
    a[1] += 0x8000000000000 - 1;
    a[2] += 0x8000000000000 - 1;
    a[3] += 0x8000000000000 - 1;
    a[4] += 0x8000000000000 - 1;

    // Now between 2^255 and 2^256-20, and offset by 2^255.

    a[1] += a[0] >> 51;
    a[2] += a[1] >> 51;
    a[3] += a[2] >> 51;
    a[4] += a[3] >> 51;
    a[0] &= 0x7FFFFFFFFFFFF;
    a[1] &= 0x7FFFFFFFFFFFF;
    a[2] &= 0x7FFFFFFFFFFFF;
    a[3] &= 0x7FFFFFFFFFFFF;
    a[4] &= 0x7FFFFFFFFFFFF;
}

// Conditionally swap `a` and `b`, depending on if `shouldSwap` is 1 or 0.
hc_UNUSED static void curve25519_cSwap(uint64_t *a, uint64_t *b, uint64_t shouldSwap) {
    uint64_t swap = -shouldSwap;
    for (int32_t i = 0; i < 5; ++i) {
        uint64_t x = swap & (a[i] ^ b[i]);
        a[i] ^= x;
        b[i] ^= x;
    }
}

hc_UNUSED
static void curve25519_add(uint64_t *out, const uint64_t *a, const uint64_t *b) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1] + (out[0] >> 51);
    out[0] &= 0x7FFFFFFFFFFFF;
    out[2] = a[2] + b[2] + (out[1] >> 51);
    out[1] &= 0x7FFFFFFFFFFFF;
    out[3] = a[3] + b[3] + (out[2] >> 51);
    out[2] &= 0x7FFFFFFFFFFFF;
    out[4] = a[4] + b[4] + (out[3] >> 51);
    out[3] &= 0x7FFFFFFFFFFFF;
    out[0] += (out[4] >> 51) * 19;
    out[4] &= 0x7FFFFFFFFFFFF;
}

static hc_INLINE void curve25519_addNoReduce(uint64_t *out, const uint64_t *a, const uint64_t *b) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
    out[3] = a[3] + b[3];
    out[4] = a[4] + b[4];
}

hc_UNUSED
static void curve25519_sub(uint64_t *out, const uint64_t *a, const uint64_t *b) {
    out[0] = a[0] + 0x0FFFFFFFFFFFDA - b[0];
    out[1] = a[1] + 0x0FFFFFFFFFFFFE - b[1] + (out[0] >> 51);
    out[0] &= 0x7FFFFFFFFFFFF;
    out[2] = a[2] + 0x0FFFFFFFFFFFFE - b[2] + (out[1] >> 51);
    out[1] &= 0x7FFFFFFFFFFFF;
    out[3] = a[3] + 0x0FFFFFFFFFFFFE - b[3] + (out[2] >> 51);
    out[2] &= 0x7FFFFFFFFFFFF;
    out[4] = a[4] + 0x0FFFFFFFFFFFFE - b[4] + (out[3] >> 51);
    out[3] &= 0x7FFFFFFFFFFFF;
    out[0] += (out[4] >> 51) * 19;
    out[4] &= 0x7FFFFFFFFFFFF;
}

static hc_INLINE void curve25519_subNoReduce(uint64_t *out, const uint64_t *a, const uint64_t *b) {
    out[0] = a[0] + 0x0FFFFFFFFFFFDA - b[0];
    out[1] = a[1] + 0x0FFFFFFFFFFFFE - b[1];
    out[2] = a[2] + 0x0FFFFFFFFFFFFE - b[2];
    out[3] = a[3] + 0x0FFFFFFFFFFFFE - b[3];
    out[4] = a[4] + 0x0FFFFFFFFFFFFE - b[4];
}

static hc_INLINE void curve25519_subNoReduceFourP(uint64_t *out, const uint64_t *a, const uint64_t *b) {
    out[0] = a[0] + 0x1FFFFFFFFFFFB4 - b[0];
    out[1] = a[1] + 0x1FFFFFFFFFFFFC - b[1];
    out[2] = a[2] + 0x1FFFFFFFFFFFFC - b[2];
    out[3] = a[3] + 0x1FFFFFFFFFFFFC - b[3];
    out[4] = a[4] + 0x1FFFFFFFFFFFFC - b[4];
}

hc_UNUSED
static void curve25519_mulScalar(uint64_t *out, const uint64_t *a, uint64_t scalar) {
    uint128_t x = hc_MUL128_64x64(a[0], scalar);
    out[0] = (uint64_t)x & 0x7FFFFFFFFFFFF;
    x = hc_MUL128_64x64(a[1], scalar) + (uint64_t)(x >> 51);
    out[1] = (uint64_t)x & 0x7FFFFFFFFFFFF;
    x = hc_MUL128_64x64(a[2], scalar) + (uint64_t)(x >> 51);
    out[2] = (uint64_t)x & 0x7FFFFFFFFFFFF;
    x = hc_MUL128_64x64(a[3], scalar) + (uint64_t)(x >> 51);
    out[3] = (uint64_t)x & 0x7FFFFFFFFFFFF;
    x = hc_MUL128_64x64(a[4], scalar) + (uint64_t)(x >> 51);
    out[4] = (uint64_t)x & 0x7FFFFFFFFFFFF;
    out[0] += (uint64_t)(x >> 51) * 19;
}

hc_UNUSED
static void curve25519_mul(uint64_t *out, const uint64_t *a, const uint64_t *b) {
    uint64_t r0 = b[0];
    uint64_t r1 = b[1];
    uint64_t r2 = b[2];
    uint64_t r3 = b[3];
    uint64_t r4 = b[4];

    uint64_t s0 = a[0];
    uint64_t s1 = a[1];
    uint64_t s2 = a[2];
    uint64_t s3 = a[3];
    uint64_t s4 = a[4];

    uint128_t t[5] = {
        hc_MUL128_64x64(r0, s0),
        hc_MUL128_64x64(r0, s1) + hc_MUL128_64x64(r1, s0),
        hc_MUL128_64x64(r0, s2) + hc_MUL128_64x64(r2, s0) + hc_MUL128_64x64(r1, s1),
        hc_MUL128_64x64(r0, s3) + hc_MUL128_64x64(r3, s0) + hc_MUL128_64x64(r1, s2) + hc_MUL128_64x64(r2, s1),
        hc_MUL128_64x64(r0, s4) + hc_MUL128_64x64(r4, s0) + hc_MUL128_64x64(r3, s1) + hc_MUL128_64x64(r1, s3) + hc_MUL128_64x64(r2, s2)
    };

    r4 *= 19;
    r1 *= 19;
    r2 *= 19;
    r3 *= 19;

    t[0] += hc_MUL128_64x64(r4, s1) + hc_MUL128_64x64(r1, s4) + hc_MUL128_64x64(r2, s3) + hc_MUL128_64x64(r3, s2);
    t[1] += hc_MUL128_64x64(r4, s2) + hc_MUL128_64x64(r2, s4) + hc_MUL128_64x64(r3, s3);
    t[2] += hc_MUL128_64x64(r4, s3) + hc_MUL128_64x64(r3, s4);
    t[3] += hc_MUL128_64x64(r4, s4);

    r0 = (uint64_t)t[0] & 0x7FFFFFFFFFFFF;
    t[1] += (uint64_t)(t[0] >> 51);
    r1 = (uint64_t)t[1] & 0x7FFFFFFFFFFFF;
    t[2] += (uint64_t)(t[1] >> 51);
    r2 = (uint64_t)t[2] & 0x7FFFFFFFFFFFF;
    t[3] += (uint64_t)(t[2] >> 51);
    r3 = (uint64_t)t[3] & 0x7FFFFFFFFFFFF;
    t[4] += (uint64_t)(t[3] >> 51);
    r4 = (uint64_t)t[4] & 0x7FFFFFFFFFFFF;

    r0 += (uint64_t)(t[4] >> 51) * 19;
    r1 += (r0 >> 51);
    r0 = r0 & 0x7FFFFFFFFFFFF;

    out[0] = r0;
    out[1] = r1;
    out[2] = r2;
    out[3] = r3;
    out[4] = r4;
}

hc_UNUSED
static void curve25519_squareN(uint64_t *out, const uint64_t *a, uint64_t n) {
    uint64_t r0 = a[0];
    uint64_t r1 = a[1];
    uint64_t r2 = a[2];
    uint64_t r3 = a[3];
    uint64_t r4 = a[4];

    do {
        uint64_t d0 = r0 * 2;
        uint64_t d1 = r1 * 2;
        uint64_t d2 = r2 * 2 * 19;
        uint64_t d419 = r4 * 19;
        uint64_t d4 = d419 * 2;

        uint128_t t[5] = {
            hc_MUL128_64x64(r0, r0) + hc_MUL128_64x64(d4, r1) + hc_MUL128_64x64(d2, r3),
            hc_MUL128_64x64(d0, r1) + hc_MUL128_64x64(d4, r2) + hc_MUL128_64x64(r3, (r3 * 19)),
            hc_MUL128_64x64(d0, r2) + hc_MUL128_64x64(r1, r1) + hc_MUL128_64x64(d4, r3),
            hc_MUL128_64x64(d0, r3) + hc_MUL128_64x64(d1, r2) + hc_MUL128_64x64(r4, d419),
            hc_MUL128_64x64(d0, r4) + hc_MUL128_64x64(d1, r3) + hc_MUL128_64x64(r2, r2)
        };
        r0 = (uint64_t)t[0] & 0x7FFFFFFFFFFFF;
        t[1] += (uint64_t)(t[0] >> 51);
        r1 = (uint64_t)t[1] & 0x7FFFFFFFFFFFF;
        t[2] += (uint64_t)(t[1] >> 51);
        r2 = (uint64_t)t[2] & 0x7FFFFFFFFFFFF;
        t[3] += (uint64_t)(t[2] >> 51);
        r3 = (uint64_t)t[3] & 0x7FFFFFFFFFFFF;
        t[4] += (uint64_t)(t[3] >> 51);
        r4 = (uint64_t)t[4] & 0x7FFFFFFFFFFFF;
        r0 += (uint64_t)(t[4] >> 51) * 19;
        r1 += (r0 >> 51);
        r0 = r0 & 0x7FFFFFFFFFFFF;
    } while (--n);

    out[0] = r0;
    out[1] = r1;
    out[2] = r2;
    out[3] = r3;
    out[4] = r4;
}

// Calculate a = z^11, b = z^(2^250 - 2^0)
static void _curve25519_pow11PowTwo250mtwo0(uint64_t *a, uint64_t *b, const uint64_t *z) {
    uint64_t t0[5], c[5];
    /* 2 */ curve25519_squareN(a, z, 1);
    /* 8 */ curve25519_squareN(&t0[0], a, 2);
    /* 9 */ curve25519_mul(b, &t0[0], z);
    /* 11 */ curve25519_mul(a, b, a);
    /* 22 */ curve25519_squareN(&t0[0], a, 1);
    /* 2^5 - 2^0 = 31 */ curve25519_mul(b, &t0[0], b);
    /* 2^10 - 2^5 */ curve25519_squareN(&t0[0], b, 5);
    /* 2^10 - 2^0 */ curve25519_mul(b, &t0[0], b);
    /* 2^20 - 2^10 */ curve25519_squareN(&t0[0], b, 10);
    /* 2^20 - 2^0 */ curve25519_mul(&c[0], &t0[0], b);
    /* 2^40 - 2^20 */ curve25519_squareN(&t0[0], &c[0], 20);
    /* 2^40 - 2^0 */ curve25519_mul(&t0[0], &t0[0], &c[0]);
    /* 2^50 - 2^10 */ curve25519_squareN(&t0[0], &t0[0], 10);
    /* 2^50 - 2^0 */ curve25519_mul(b, &t0[0], b);
    /* 2^100 - 2^50 */ curve25519_squareN(&t0[0], b, 50);
    /* 2^100 - 2^0 */ curve25519_mul(&c[0], &t0[0], b);
    /* 2^200 - 2^100 */ curve25519_squareN(&t0[0], &c[0], 100);
    /* 2^200 - 2^0 */ curve25519_mul(&t0[0], &t0[0], &c[0]);
    /* 2^250 - 2^50 */ curve25519_squareN(&t0[0], &t0[0], 50);
    /* 2^250 - 2^0 */ curve25519_mul(b, &t0[0], b);
}

// Calculate inverse of z: z^(p - 2) = z^(2^255 - 21)
hc_UNUSED static void curve25519_invert(uint64_t *out, const uint64_t *z) {
    uint64_t a[5], b[5];
    /* 2^11 and 2^250 - 2^0 */ _curve25519_pow11PowTwo250mtwo0(&a[0], &b[0], z);
    /* 2^255 - 2^5 */ curve25519_squareN(&b[0], &b[0], 5);
    /* 2^255 - 21 */ curve25519_mul(out, &b[0], &a[0]);
}

// Calculate z^((p-5)/8) = z^(2^252 - 3)
hc_UNUSED static void curve25519_powTwo252m3(uint64_t *out, const uint64_t *z) {
    uint64_t a[5], b[5];
    /* 2^11 and 2^250 - 2^0 */ _curve25519_pow11PowTwo250mtwo0(&a[0], &b[0], z);
    /* 2^252 - 2^2 */ curve25519_squareN(&b[0], &b[0], 2);
    /* 2^252 - 3 */ curve25519_mul(out, &b[0], z);
}

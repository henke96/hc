// Code released into the public domain.
// By Adam Langley <agl@imperialviolet.org>
// Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>

/* Sum two numbers: output += in */
static hc_INLINE void _curve25519_fsum(uint64_t *output, const uint64_t *in) {
    output[0] += in[0];
    output[1] += in[1];
    output[2] += in[2];
    output[3] += in[3];
    output[4] += in[4];
}

/* Find the difference of two numbers: output = in - output
 * (note the order of the arguments!)
 *
 * Assumes that out[i] < 2**52
 * On return, out[i] < 2**55
 */
static hc_INLINE void _curve25519_fdifference_backwards(uint64_t *out, const uint64_t *in) {
    out[0] = in[0] + (uint64_t)0x3FFFFFFFFFFF68 - out[0];
    out[1] = in[1] + (uint64_t)0x3FFFFFFFFFFFF8 - out[1];
    out[2] = in[2] + (uint64_t)0x3FFFFFFFFFFFF8 - out[2];
    out[3] = in[3] + (uint64_t)0x3FFFFFFFFFFFF8 - out[3];
    out[4] = in[4] + (uint64_t)0x3FFFFFFFFFFFF8 - out[4];
}

/* Multiply a number by a scalar: output = in * scalar */
static hc_INLINE void _curve25519_fscalar_product(uint64_t *output, const uint64_t *in, const uint64_t scalar) {
    uint128_t a = ((uint128_t)in[0]) * scalar;
    output[0] = ((uint64_t)a) & 0x7FFFFFFFFFFFF;

    a = ((uint128_t)in[1]) * scalar + ((uint64_t)(a >> 51));
    output[1] = ((uint64_t)a) & 0x7FFFFFFFFFFFF;

    a = ((uint128_t)in[2]) * scalar + ((uint64_t)(a >> 51));
    output[2] = ((uint64_t)a) & 0x7FFFFFFFFFFFF;

    a = ((uint128_t)in[3]) * scalar + ((uint64_t)(a >> 51));
    output[3] = ((uint64_t)a) & 0x7FFFFFFFFFFFF;

    a = ((uint128_t)in[4]) * scalar + ((uint64_t)(a >> 51));
    output[4] = ((uint64_t)a) & 0x7FFFFFFFFFFFF;

    output[0] += (a >> 51) * 19;
}

/* Multiply two numbers: output = in2 * in
 *
 * output must be distinct to both inputs. The inputs are reduced coefficient
 * form, the output is not.
 *
 * Assumes that in[i] < 2**55 and likewise for in2.
 * On return, output[i] < 2**52
 */
static void _curve25519_fmul(uint64_t *output, const uint64_t *in2, const uint64_t *in) {
    uint64_t r0 = in[0];
    uint64_t r1 = in[1];
    uint64_t r2 = in[2];
    uint64_t r3 = in[3];
    uint64_t r4 = in[4];

    uint64_t s0 = in2[0];
    uint64_t s1 = in2[1];
    uint64_t s2 = in2[2];
    uint64_t s3 = in2[3];
    uint64_t s4 = in2[4];

    uint128_t t[5] = {
        ((uint128_t)r0) * s0,
        ((uint128_t)r0) * s1 + ((uint128_t)r1) * s0,
        ((uint128_t)r0) * s2 + ((uint128_t)r2) * s0 + ((uint128_t)r1) * s1,
        ((uint128_t)r0) * s3 + ((uint128_t)r3) * s0 + ((uint128_t)r1) * s2 + ((uint128_t)r2) * s1,
        ((uint128_t)r0) * s4 + ((uint128_t)r4) * s0 + ((uint128_t)r3) * s1 + ((uint128_t)r1) * s3 + ((uint128_t)r2) * s2
    };

    r4 *= 19;
    r1 *= 19;
    r2 *= 19;
    r3 *= 19;

    t[0] += ((uint128_t)r4) * s1 + ((uint128_t)r1) * s4 + ((uint128_t)r2) * s3 + ((uint128_t)r3) * s2;
    t[1] += ((uint128_t)r4) * s2 + ((uint128_t)r2) * s4 + ((uint128_t)r3) * s3;
    t[2] += ((uint128_t)r4) * s3 + ((uint128_t)r3) * s4;
    t[3] += ((uint128_t)r4) * s4;

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
    r2 += (r1 >> 51);
    r1 = r1 & 0x7FFFFFFFFFFFF;

    output[0] = r0;
    output[1] = r1;
    output[2] = r2;
    output[3] = r3;
    output[4] = r4;
}

static void _curve25519_fsquare_times(uint64_t *output, const uint64_t *in, uint64_t count) {
    uint64_t r0 = in[0];
    uint64_t r1 = in[1];
    uint64_t r2 = in[2];
    uint64_t r3 = in[3];
    uint64_t r4 = in[4];

    do {
        uint64_t d0 = r0 * 2;
        uint64_t d1 = r1 * 2;
        uint64_t d2 = r2 * 2 * 19;
        uint64_t d419 = r4 * 19;
        uint64_t d4 = d419 * 2;

        uint128_t t[5] = {
            ((uint128_t)r0) * r0 + ((uint128_t)d4) * r1 + (((uint128_t)d2) * (r3     )),
            ((uint128_t)d0) * r1 + ((uint128_t)d4) * r2 + (((uint128_t)r3) * (r3 * 19)),
            ((uint128_t)d0) * r2 + ((uint128_t)r1) * r1 + (((uint128_t)d4) * (r3     )),
            ((uint128_t)d0) * r3 + ((uint128_t)d1) * r2 + (((uint128_t)r4) * (d419   )),
            ((uint128_t)d0) * r4 + ((uint128_t)d1) * r3 + (((uint128_t)r2) * (r2     ))
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
        r2 += (r1 >> 51);
        r1 = r1 & 0x7FFFFFFFFFFFF;
    } while (--count);

    output[0] = r0;
    output[1] = r1;
    output[2] = r2;
    output[3] = r3;
    output[4] = r4;
}

/* Load a little-endian 64-bit number */
static hc_INLINE uint64_t _curve25519_load_limb(const uint8_t *in) {
    uint64_t x;
    hc_MEMCPY(&x, in, 8);
    return x;
}

static hc_INLINE void _curve25519_store_limb(uint8_t *out, uint64_t in) {
    hc_MEMCPY(out, &in, 8);
}

/* Take a little-endian, 32-byte number and expand it into polynomial form */
static void _curve25519_fexpand(uint64_t *output, const uint8_t *in) {
    output[0] = _curve25519_load_limb(in) & 0x7FFFFFFFFFFFF;
    output[1] = (_curve25519_load_limb(in + 6) >> 3) & 0x7FFFFFFFFFFFF;
    output[2] = (_curve25519_load_limb(in + 12) >> 6) & 0x7FFFFFFFFFFFF;
    output[3] = (_curve25519_load_limb(in + 19) >> 1) & 0x7FFFFFFFFFFFF;
    output[4] = (_curve25519_load_limb(in + 24) >> 12) & 0x7FFFFFFFFFFFF;
}

/* Take a fully reduced polynomial form number and contract it into a
 * little-endian, 32-byte array
 */
static void _curve25519_fcontract(uint8_t *output, const uint64_t *input) {
    uint128_t t[5] = {
        input[0],
        input[1],
        input[2],
        input[3],
        input[4]
    };

    t[1] += t[0] >> 51; t[0] &= 0x7FFFFFFFFFFFF;
    t[2] += t[1] >> 51; t[1] &= 0x7FFFFFFFFFFFF;
    t[3] += t[2] >> 51; t[2] &= 0x7FFFFFFFFFFFF;
    t[4] += t[3] >> 51; t[3] &= 0x7FFFFFFFFFFFF;
    t[0] += 19 * (t[4] >> 51); t[4] &= 0x7FFFFFFFFFFFF;

    t[1] += t[0] >> 51; t[0] &= 0x7FFFFFFFFFFFF;
    t[2] += t[1] >> 51; t[1] &= 0x7FFFFFFFFFFFF;
    t[3] += t[2] >> 51; t[2] &= 0x7FFFFFFFFFFFF;
    t[4] += t[3] >> 51; t[3] &= 0x7FFFFFFFFFFFF;
    t[0] += 19 * (t[4] >> 51); t[4] &= 0x7FFFFFFFFFFFF;

    /* now t is between 0 and 2^255-1, properly carried. */
    /* case 1: between 0 and 2^255-20. case 2: between 2^255-19 and 2^255-1. */

    t[0] += 19;

    t[1] += t[0] >> 51; t[0] &= 0x7FFFFFFFFFFFF;
    t[2] += t[1] >> 51; t[1] &= 0x7FFFFFFFFFFFF;
    t[3] += t[2] >> 51; t[2] &= 0x7FFFFFFFFFFFF;
    t[4] += t[3] >> 51; t[3] &= 0x7FFFFFFFFFFFF;
    t[0] += 19 * (t[4] >> 51); t[4] &= 0x7FFFFFFFFFFFF;

    /* now between 19 and 2^255-1 in both cases, and offset by 19. */

    t[0] += 0x8000000000000 - 19;
    t[1] += 0x8000000000000 - 1;
    t[2] += 0x8000000000000 - 1;
    t[3] += 0x8000000000000 - 1;
    t[4] += 0x8000000000000 - 1;

    /* now between 2^255 and 2^256-20, and offset by 2^255. */

    t[1] += t[0] >> 51;
    t[2] += t[1] >> 51;
    t[3] += t[2] >> 51;
    t[4] += t[3] >> 51;
    uint64_t t0 = (uint64_t)t[0] & 0x7FFFFFFFFFFFF;
    uint64_t t1 = (uint64_t)t[1] & 0x7FFFFFFFFFFFF;
    uint64_t t2 = (uint64_t)t[2] & 0x7FFFFFFFFFFFF;
    uint64_t t3 = (uint64_t)t[3] & 0x7FFFFFFFFFFFF;
    uint64_t t4 = (uint64_t)t[4] & 0x7FFFFFFFFFFFF;

    _curve25519_store_limb(&output[0], t0 | (t1 << 51));
    _curve25519_store_limb(&output[8], (t1 >> 13) | (t2 << 38));
    _curve25519_store_limb(&output[16], (t2 >> 26) | (t3 << 25));
    _curve25519_store_limb(&output[24], (t3 >> 39) | (t4 << 12));
}

/* Input: Q, Q', Q-Q'
 * Output: 2Q, Q+Q'
 *
 *   x2 z3: long form
 *   x3 z3: long form
 *   x z: short form, destroyed
 *   xprime zprime: short form, destroyed
 *   qmqp: short form, preserved
 */
static void _curve25519_fmonty(
    uint64_t *x2, uint64_t *z2, /* output 2Q */
    uint64_t *x3, uint64_t *z3, /* output Q + Q' */
    uint64_t *x, uint64_t *z,   /* input Q */
    uint64_t *xprime, uint64_t *zprime, /* input Q' */
    const uint64_t *qmqp /* input Q - Q' */
) {
    uint64_t origx[5], origxprime[5], zzz[5], xx[5], zz[5], xxprime[5], zzprime[5], zzzprime[5];

    hc_MEMCPY(&origx[0], x, 5 * sizeof(uint64_t));
    _curve25519_fsum(x, z);
    _curve25519_fdifference_backwards(z, &origx[0]);  // does x - z

    hc_MEMCPY(&origxprime[0], xprime, sizeof(uint64_t) * 5);
    _curve25519_fsum(xprime, zprime);
    _curve25519_fdifference_backwards(zprime, &origxprime[0]);
    _curve25519_fmul(&xxprime[0], xprime, z);
    _curve25519_fmul(&zzprime[0], x, zprime);
    hc_MEMCPY(&origxprime[0], &xxprime[0], sizeof(uint64_t) * 5);
    _curve25519_fsum(&xxprime[0], &zzprime[0]);
    _curve25519_fdifference_backwards(&zzprime[0], &origxprime[0]);
    _curve25519_fsquare_times(x3, &xxprime[0], 1);
    _curve25519_fsquare_times(&zzzprime[0], &zzprime[0], 1);
    _curve25519_fmul(z3, &zzzprime[0], qmqp);

    _curve25519_fsquare_times(&xx[0], x, 1);
    _curve25519_fsquare_times(zz, z, 1);
    _curve25519_fmul(x2, &xx[0], zz);
    _curve25519_fdifference_backwards(zz, &xx[0]);  // does zz = xx - zz
    _curve25519_fscalar_product(&zzz[0], zz, 121665);
    _curve25519_fsum(&zzz[0], &xx[0]);
    _curve25519_fmul(z2, zz, &zzz[0]);
}

// -----------------------------------------------------------------------------
// Maybe swap the contents of two limb arrays (@a and @b), each @len elements
// long. Perform the swap iff @swap is non-zero.
//
// This function performs the swap without leaking any side-channel
// information.
// -----------------------------------------------------------------------------
static void _curve25519_swap_conditional(uint64_t *a, uint64_t *b, uint64_t iswap) {
    const uint64_t swap = -iswap;
    for (int32_t i = 0; i < 5; ++i) {
        const uint64_t x = swap & (a[i] ^ b[i]);
        a[i] ^= x;
        b[i] ^= x;
    }
}

/* Calculates nQ where Q is the x-coordinate of a point on the curve
 *
 *   resultx/resultz: the x coordinate of the resulting curve point (short form)
 *   n: a little endian, 32-byte number
 *   q: a point of the curve (short form)
 */
static void _curve25519_cmult(uint64_t *resultx, uint64_t *resultz, const uint8_t *n, const uint64_t *q) {
    uint64_t a[5] = {0}, b[5] = {1}, c[5] = {1}, d[5] = {0};
    uint64_t *nqpqx = &a[0], *nqpqz = &b[0], *nqx = &c[0], *nqz = &d[0];
    uint64_t e[5] = {0}, f[5] = {1}, g[5] = {0}, h[5] = {1};
    uint64_t *nqpqx2 = &e[0], *nqpqz2 = &f[0], *nqx2 = &g[0], *nqz2 = &h[0];

    hc_MEMCPY(nqpqx, q, sizeof(uint64_t) * 5);

    for (int32_t i = 0; i < 32; ++i) {
        uint8_t byte = n[31 - i];
        for (int32_t j = 0; j < 8; ++j) {
            const uint64_t bit = byte >> 7;

            _curve25519_swap_conditional(nqx, nqpqx, bit);
            _curve25519_swap_conditional(nqz, nqpqz, bit);
            _curve25519_fmonty(
                nqx2, nqz2,
                nqpqx2, nqpqz2,
                nqx, nqz,
                nqpqx, nqpqz,
                q
            );
            _curve25519_swap_conditional(nqx2, nqpqx2, bit);
            _curve25519_swap_conditional(nqz2, nqpqz2, bit);

            uint64_t *t = nqx;
            nqx = nqx2;
            nqx2 = t;
            t = nqz;
            nqz = nqz2;
            nqz2 = t;
            t = nqpqx;
            nqpqx = nqpqx2;
            nqpqx2 = t;
            t = nqpqz;
            nqpqz = nqpqz2;
            nqpqz2 = t;

            byte <<= 1;
        }
    }

    hc_MEMCPY(resultx, nqx, sizeof(uint64_t) * 5);
    hc_MEMCPY(resultz, nqz, sizeof(uint64_t) * 5);
}


// -----------------------------------------------------------------------------
// Shamelessly copied from djb's code, tightened a little
// -----------------------------------------------------------------------------
static void _curve25519_crecip(uint64_t *out, const uint64_t *z) {
    uint64_t a[5], t0[5], b[5], c[5];

    /* 2 */ _curve25519_fsquare_times(&a[0], z, 1); // a = 2
    /* 8 */ _curve25519_fsquare_times(&t0[0], &a[0], 2);
    /* 9 */ _curve25519_fmul(&b[0], &t0[0], z); // b = 9
    /* 11 */ _curve25519_fmul(&a[0], &b[0], &a[0]); // a = 11
    /* 22 */ _curve25519_fsquare_times(&t0[0], &a[0], 1);
    /* 2^5 - 2^0 = 31 */ _curve25519_fmul(&b[0], &t0[0], &b[0]);
    /* 2^10 - 2^5 */ _curve25519_fsquare_times(&t0[0], &b[0], 5);
    /* 2^10 - 2^0 */ _curve25519_fmul(&b[0], &t0[0], &b[0]);
    /* 2^20 - 2^10 */ _curve25519_fsquare_times(&t0[0], &b[0], 10);
    /* 2^20 - 2^0 */ _curve25519_fmul(&c[0], &t0[0], &b[0]);
    /* 2^40 - 2^20 */ _curve25519_fsquare_times(&t0[0], &c[0], 20);
    /* 2^40 - 2^0 */ _curve25519_fmul(&t0[0], &t0[0], &c[0]);
    /* 2^50 - 2^10 */ _curve25519_fsquare_times(&t0[0], &t0[0], 10);
    /* 2^50 - 2^0 */ _curve25519_fmul(&b[0], &t0[0], &b[0]);
    /* 2^100 - 2^50 */ _curve25519_fsquare_times(&t0[0], &b[0], 50);
    /* 2^100 - 2^0 */ _curve25519_fmul(&c[0], &t0[0], &b[0]);
    /* 2^200 - 2^100 */ _curve25519_fsquare_times(&t0[0], &c[0], 100);
    /* 2^200 - 2^0 */ _curve25519_fmul(&t0[0], &t0[0], &c[0]);
    /* 2^250 - 2^50 */ _curve25519_fsquare_times(&t0[0], &t0[0], 50);
    /* 2^250 - 2^0 */ _curve25519_fmul(&t0[0], &t0[0], &b[0]);
    /* 2^255 - 2^5 */ _curve25519_fsquare_times(&t0[0], &t0[0], 5);
    /* 2^255 - 21 */ _curve25519_fmul(out, &t0[0], &a[0]);
}

static const uint8_t curve25519_ecdhBasepoint[32] = { 9 };

static void curve25519(uint8_t *out, const uint8_t *secret, const uint8_t *public) {
    uint64_t bp[5], x[5], z[5], zmone[5];
    uint8_t e[32];

    hc_MEMCPY(&e[0], &secret[0], 32);
    e[0] &= 248;
    e[31] &= 127;
    e[31] |= 64;

    _curve25519_fexpand(bp, public);
    _curve25519_cmult(x, z, e, bp);
    _curve25519_crecip(zmone, z);
    _curve25519_fmul(z, x, zmone);
    _curve25519_fcontract(out, z);
}

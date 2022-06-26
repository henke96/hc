// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Copyright (C) 2016-2017 INRIA and Microsoft Corporation.
 * Copyright (C) 2018-2020 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 *
 * This is a machine-generated formally verified implementation of Curve25519
 * ECDH from: <https://github.com/mitls/hacl-star>. Though originally machine
 * generated, it has been tweaked to be suitable for use in the kernel. It is
 * optimized for 64-bit machines that can efficiently work with 128-bit
 * integer types.
 */

static hc_ALWAYS_INLINE uint64_t curve25519_getUnalignedLe64(const uint8_t *a) {
    uint64_t l;
    hc_MEMCPY(&l, a, sizeof(l));
    return l;
}

static hc_ALWAYS_INLINE void curve25519_putUnalignedLe64(uint64_t s, uint8_t *d) {
    hc_MEMCPY(d, &s, sizeof(s));
}

static void curve25519_memzeroExplicit(void *s, size_t count) {
    hc_MEMSET(s, 0, count);
    asm volatile("": :"r"(s) :"memory");
}

static hc_ALWAYS_INLINE void curve25519_clampSecret(uint8_t *secret) {
    secret[0] &= 248;
    secret[31] = (secret[31] & 127) | 64;
}

static hc_ALWAYS_INLINE uint64_t curve25519_u64EqMask(uint64_t a, uint64_t b) {
    uint64_t x = a ^ b;
    uint64_t minus_x = ~x + (uint64_t)1U;
    uint64_t x_or_minus_x = x | minus_x;
    uint64_t xnx = x_or_minus_x >> (uint32_t)63U;
    uint64_t c = xnx - (uint64_t)1U;
    return c;
}

static hc_ALWAYS_INLINE uint64_t curve25519_u64GteMask(uint64_t a, uint64_t b) {
    uint64_t x = a;
    uint64_t y = b;
    uint64_t x_xor_y = x ^ y;
    uint64_t x_sub_y = x - y;
    uint64_t x_sub_y_xor_y = x_sub_y ^ y;
    uint64_t q = x_xor_y | x_sub_y_xor_y;
    uint64_t x_xor_q = x ^ q;
    uint64_t x_xor_q_ = x_xor_q >> (uint32_t)63U;
    uint64_t c = x_xor_q_ - (uint64_t)1U;
    return c;
}

static hc_ALWAYS_INLINE void curve25519_moduloCarryTop(uint64_t *b) {
    uint64_t b4 = b[4];
    uint64_t b0 = b[0];
    uint64_t b4_ = b4 & 0x7ffffffffffffLLU;
    uint64_t b0_ = b0 + 19 * (b4 >> 51);
    b[4] = b4_;
    b[0] = b0_;
}

static hc_ALWAYS_INLINE void curve25519_fproductCopyFromWide_(uint64_t *output, uint128_t *input) {
    {
        uint128_t xi = input[0];
        output[0] = ((uint64_t)(xi));
    }
    {
        uint128_t xi = input[1];
        output[1] = ((uint64_t)(xi));
    }
    {
        uint128_t xi = input[2];
        output[2] = ((uint64_t)(xi));
    }
    {
        uint128_t xi = input[3];
        output[3] = ((uint64_t)(xi));
    }
    {
        uint128_t xi = input[4];
        output[4] = ((uint64_t)(xi));
    }
}

static hc_ALWAYS_INLINE void curve25519_fproductSumScalarMultiplication_(uint128_t *output, uint64_t *input, uint64_t s) {
    output[0] += (uint128_t)input[0] * s;
    output[1] += (uint128_t)input[1] * s;
    output[2] += (uint128_t)input[2] * s;
    output[3] += (uint128_t)input[3] * s;
    output[4] += (uint128_t)input[4] * s;
}

static hc_ALWAYS_INLINE void curve25519_fproductCarryWide(uint128_t *tmp) {
    {
        uint32_t ctr = 0;
        uint128_t tctr = tmp[ctr];
        uint128_t tctrp1 = tmp[ctr + 1];
        uint64_t r0 = ((uint64_t)(tctr)) & 0x7ffffffffffffLLU;
        uint128_t c = ((tctr) >> (51));
        tmp[ctr] = ((uint128_t)(r0));
        tmp[ctr + 1] = ((tctrp1) + (c));
    }
    {
        uint32_t ctr = 1;
        uint128_t tctr = tmp[ctr];
        uint128_t tctrp1 = tmp[ctr + 1];
        uint64_t r0 = ((uint64_t)(tctr)) & 0x7ffffffffffffLLU;
        uint128_t c = ((tctr) >> (51));
        tmp[ctr] = ((uint128_t)(r0));
        tmp[ctr + 1] = ((tctrp1) + (c));
    }

    {
        uint32_t ctr = 2;
        uint128_t tctr = tmp[ctr];
        uint128_t tctrp1 = tmp[ctr + 1];
        uint64_t r0 = ((uint64_t)(tctr)) & 0x7ffffffffffffLLU;
        uint128_t c = ((tctr) >> (51));
        tmp[ctr] = ((uint128_t)(r0));
        tmp[ctr + 1] = ((tctrp1) + (c));
    }
    {
        uint32_t ctr = 3;
        uint128_t tctr = tmp[ctr];
        uint128_t tctrp1 = tmp[ctr + 1];
        uint64_t r0 = ((uint64_t)(tctr)) & 0x7ffffffffffffLLU;
        uint128_t c = ((tctr) >> (51));
        tmp[ctr] = ((uint128_t)(r0));
        tmp[ctr + 1] = ((tctrp1) + (c));
    }
}

static hc_ALWAYS_INLINE void curve25519_fmulShiftReduce(uint64_t *output) {
    uint64_t tmp = output[4];
    uint64_t b0;
    {
        uint32_t ctr = 5 - 0 - 1;
        uint64_t z = output[ctr - 1];
        output[ctr] = z;
    }
    {
        uint32_t ctr = 5 - 1 - 1;
        uint64_t z = output[ctr - 1];
        output[ctr] = z;
    }
    {
        uint32_t ctr = 5 - 2 - 1;
        uint64_t z = output[ctr - 1];
        output[ctr] = z;
    }
    {
        uint32_t ctr = 5 - 3 - 1;
        uint64_t z = output[ctr - 1];
        output[ctr] = z;
    }
    output[0] = tmp;
    b0 = output[0];
    output[0] = 19 * b0;
}

static hc_ALWAYS_INLINE void curve25519_fmulMulShiftReduce_(uint128_t *output, uint64_t *input, uint64_t *input21) {
    {
        uint64_t input2i = input21[0];
        curve25519_fproductSumScalarMultiplication_(output, input, input2i);
        curve25519_fmulShiftReduce(input);
    }
    {
        uint64_t input2i = input21[1];
        curve25519_fproductSumScalarMultiplication_(output, input, input2i);
        curve25519_fmulShiftReduce(input);
    }
    {
        uint64_t input2i = input21[2];
        curve25519_fproductSumScalarMultiplication_(output, input, input2i);
        curve25519_fmulShiftReduce(input);
    }
    {
        uint64_t input2i = input21[3];
        curve25519_fproductSumScalarMultiplication_(output, input, input2i);
        curve25519_fmulShiftReduce(input);
    }
    uint32_t i = 4;
    uint64_t input2i = input21[i];
    curve25519_fproductSumScalarMultiplication_(output, input, input2i);
}

static hc_ALWAYS_INLINE void curve25519_fmulFmul(uint64_t *output, uint64_t *input, uint64_t *input21) {
    uint64_t tmp[5] = { input[0], input[1], input[2], input[3], input[4] };
    {
        uint128_t b4;
        uint128_t b0;
        uint128_t b4_;
        uint128_t b0_;
        uint64_t i0;
        uint64_t i1;
        uint64_t i0_;
        uint64_t i1_;
        uint128_t t[5] = { 0 };
        curve25519_fmulMulShiftReduce_(t, tmp, input21);
        curve25519_fproductCarryWide(t);
        b4 = t[4];
        b0 = t[0];
        b4_ = ((b4) & (((uint128_t)(0x7ffffffffffffLLU))));
        b0_ = ((b0) + (((uint128_t)(19) * (((uint64_t)(((b4) >> (51))))))));
        t[4] = b4_;
        t[0] = b0_;
        curve25519_fproductCopyFromWide_(output, t);
        i0 = output[0];
        i1 = output[1];
        i0_ = i0 & 0x7ffffffffffffLLU;
        i1_ = i1 + (i0 >> 51);
        output[0] = i0_;
        output[1] = i1_;
    }
}

static hc_ALWAYS_INLINE void curve25519_fsquareFsquare__(uint128_t *tmp, uint64_t *output) {
    uint64_t r0 = output[0];
    uint64_t r1 = output[1];
    uint64_t r2 = output[2];
    uint64_t r3 = output[3];
    uint64_t r4 = output[4];
    uint64_t d0 = r0 * 2;
    uint64_t d1 = r1 * 2;
    uint64_t d2 = r2 * 2 * 19;
    uint64_t d419 = r4 * 19;
    uint64_t d4 = d419 * 2;
    uint128_t s0 = ((((((uint128_t)(r0) * (r0))) + (((uint128_t)(d4) * (r1))))) + (((uint128_t)(d2) * (r3))));
    uint128_t s1 = ((((((uint128_t)(d0) * (r1))) + (((uint128_t)(d4) * (r2))))) + (((uint128_t)(r3 * 19) * (r3))));
    uint128_t s2 = ((((((uint128_t)(d0) * (r2))) + (((uint128_t)(r1) * (r1))))) + (((uint128_t)(d4) * (r3))));
    uint128_t s3 = ((((((uint128_t)(d0) * (r3))) + (((uint128_t)(d1) * (r2))))) + (((uint128_t)(r4) * (d419))));
    uint128_t s4 = ((((((uint128_t)(d0) * (r4))) + (((uint128_t)(d1) * (r3))))) + (((uint128_t)(r2) * (r2))));
    tmp[0] = s0;
    tmp[1] = s1;
    tmp[2] = s2;
    tmp[3] = s3;
    tmp[4] = s4;
}

static hc_ALWAYS_INLINE void curve25519_fsquareFsquare_(uint128_t *tmp, uint64_t *output) {
    uint128_t b4;
    uint128_t b0;
    uint128_t b4_;
    uint128_t b0_;
    uint64_t i0;
    uint64_t i1;
    uint64_t i0_;
    uint64_t i1_;
    curve25519_fsquareFsquare__(tmp, output);
    curve25519_fproductCarryWide(tmp);
    b4 = tmp[4];
    b0 = tmp[0];
    b4_ = ((b4) & (((uint128_t)(0x7ffffffffffffLLU))));
    b0_ = ((b0) + (((uint128_t)(19) * (((uint64_t)(((b4) >> (51))))))));
    tmp[4] = b4_;
    tmp[0] = b0_;
    curve25519_fproductCopyFromWide_(output, tmp);
    i0 = output[0];
    i1 = output[1];
    i0_ = i0 & 0x7ffffffffffffLLU;
    i1_ = i1 + (i0 >> 51);
    output[0] = i0_;
    output[1] = i1_;
}

static hc_ALWAYS_INLINE void curve25519_fsquareFsquareTimes_(uint64_t *output, uint128_t *tmp, uint32_t count1) {
    uint32_t i;
    curve25519_fsquareFsquare_(tmp, output);
    for (i = 1; i < count1; ++i) curve25519_fsquareFsquare_(tmp, output);
}

static hc_ALWAYS_INLINE void curve25519_fsquareFsquareTimes(uint64_t *output, uint64_t *input, uint32_t count1) {
    uint128_t t[5];
    hc_MEMCPY(output, input, 5 * sizeof(*input));
    curve25519_fsquareFsquareTimes_(output, t, count1);
}

static hc_ALWAYS_INLINE void curve25519_fsquareFsquareTimesInplace(uint64_t *output, uint32_t count1) {
    uint128_t t[5];
    curve25519_fsquareFsquareTimes_(output, t, count1);
}

static hc_ALWAYS_INLINE void curve25519_crecip_crecip(uint64_t *out, uint64_t *z) {
    uint64_t buf[20] = { 0 };
    uint64_t *a0 = buf;
    uint64_t *t00 = buf + 5;
    uint64_t *b0 = buf + 10;
    uint64_t *t01;
    uint64_t *b1;
    uint64_t *c0;
    uint64_t *a;
    uint64_t *t0;
    uint64_t *b;
    uint64_t *c;
    curve25519_fsquareFsquareTimes(a0, z, 1);
    curve25519_fsquareFsquareTimes(t00, a0, 2);
    curve25519_fmulFmul(b0, t00, z);
    curve25519_fmulFmul(a0, b0, a0);
    curve25519_fsquareFsquareTimes(t00, a0, 1);
    curve25519_fmulFmul(b0, t00, b0);
    curve25519_fsquareFsquareTimes(t00, b0, 5);
    t01 = buf + 5;
    b1 = buf + 10;
    c0 = buf + 15;
    curve25519_fmulFmul(b1, t01, b1);
    curve25519_fsquareFsquareTimes(t01, b1, 10);
    curve25519_fmulFmul(c0, t01, b1);
    curve25519_fsquareFsquareTimes(t01, c0, 20);
    curve25519_fmulFmul(t01, t01, c0);
    curve25519_fsquareFsquareTimesInplace(t01, 10);
    curve25519_fmulFmul(b1, t01, b1);
    curve25519_fsquareFsquareTimes(t01, b1, 50);
    a = buf;
    t0 = buf + 5;
    b = buf + 10;
    c = buf + 15;
    curve25519_fmulFmul(c, t0, b);
    curve25519_fsquareFsquareTimes(t0, c, 100);
    curve25519_fmulFmul(t0, t0, c);
    curve25519_fsquareFsquareTimesInplace(t0, 50);
    curve25519_fmulFmul(t0, t0, b);
    curve25519_fsquareFsquareTimesInplace(t0, 5);
    curve25519_fmulFmul(out, t0, a);
}

static hc_ALWAYS_INLINE void curve25519_fsum(uint64_t *a, uint64_t *b) {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
    a[3] += b[3];
    a[4] += b[4];
}

static hc_ALWAYS_INLINE void curve25519_fdifference(uint64_t *a, uint64_t *b) {
    uint64_t tmp[5] = { 0 };
    uint64_t b0;
    uint64_t b1;
    uint64_t b2;
    uint64_t b3;
    uint64_t b4;
    hc_MEMCPY(tmp, b, 5 * sizeof(*b));
    b0 = tmp[0];
    b1 = tmp[1];
    b2 = tmp[2];
    b3 = tmp[3];
    b4 = tmp[4];
    tmp[0] = b0 + 0x3fffffffffff68LLU;
    tmp[1] = b1 + 0x3ffffffffffff8LLU;
    tmp[2] = b2 + 0x3ffffffffffff8LLU;
    tmp[3] = b3 + 0x3ffffffffffff8LLU;
    tmp[4] = b4 + 0x3ffffffffffff8LLU;
    {
        uint64_t xi = a[0];
        uint64_t yi = tmp[0];
        a[0] = yi - xi;
    }
    {
        uint64_t xi = a[1];
        uint64_t yi = tmp[1];
        a[1] = yi - xi;
    }
    {
        uint64_t xi = a[2];
        uint64_t yi = tmp[2];
        a[2] = yi - xi;
    }
    {
        uint64_t xi = a[3];
        uint64_t yi = tmp[3];
        a[3] = yi - xi;
    }
    {
        uint64_t xi = a[4];
        uint64_t yi = tmp[4];
        a[4] = yi - xi;
    }
}

static hc_ALWAYS_INLINE void curve25519_fscalar(uint64_t *output, uint64_t *b, uint64_t s) {
    uint128_t tmp[5];
    uint128_t b4;
    uint128_t b0;
    uint128_t b4_;
    uint128_t b0_;
    {
        uint64_t xi = b[0];
        tmp[0] = ((uint128_t)(xi) * (s));
    }
    {
        uint64_t xi = b[1];
        tmp[1] = ((uint128_t)(xi) * (s));
    }
    {
        uint64_t xi = b[2];
        tmp[2] = ((uint128_t)(xi) * (s));
    }
    {
        uint64_t xi = b[3];
        tmp[3] = ((uint128_t)(xi) * (s));
    }
    {
        uint64_t xi = b[4];
        tmp[4] = ((uint128_t)(xi) * (s));
    }
    curve25519_fproductCarryWide(tmp);
    b4 = tmp[4];
    b0 = tmp[0];
    b4_ = ((b4) & (((uint128_t)(0x7ffffffffffffLLU))));
    b0_ = ((b0) + (((uint128_t)(19) * (((uint64_t)(((b4) >> (51))))))));
    tmp[4] = b4_;
    tmp[0] = b0_;
    curve25519_fproductCopyFromWide_(output, tmp);
}

static hc_ALWAYS_INLINE void curve25519_fmul(uint64_t *output, uint64_t *a, uint64_t *b) {
    curve25519_fmulFmul(output, a, b);
}

static hc_ALWAYS_INLINE void curve25519_crecip(uint64_t *output, uint64_t *input) {
    curve25519_crecip_crecip(output, input);
}

static hc_ALWAYS_INLINE void curve25519_pointSwapConditionalStep(uint64_t *a, uint64_t *b, uint64_t swap1, uint32_t ctr) {
    uint32_t i = ctr - 1;
    uint64_t ai = a[i];
    uint64_t bi = b[i];
    uint64_t x = swap1 & (ai ^ bi);
    uint64_t ai1 = ai ^ x;
    uint64_t bi1 = bi ^ x;
    a[i] = ai1;
    b[i] = bi1;
}

static hc_ALWAYS_INLINE void curve25519_pointSwapConditional5(uint64_t *a, uint64_t *b, uint64_t swap1) {
    curve25519_pointSwapConditionalStep(a, b, swap1, 5);
    curve25519_pointSwapConditionalStep(a, b, swap1, 4);
    curve25519_pointSwapConditionalStep(a, b, swap1, 3);
    curve25519_pointSwapConditionalStep(a, b, swap1, 2);
    curve25519_pointSwapConditionalStep(a, b, swap1, 1);
}

static hc_ALWAYS_INLINE void curve25519_pointSwapConditional(uint64_t *a, uint64_t *b, uint64_t iswap) {
    uint64_t swap1 = 0 - iswap;
    curve25519_pointSwapConditional5(a, b, swap1);
    curve25519_pointSwapConditional5(a + 5, b + 5, swap1);
}

static hc_ALWAYS_INLINE void curve25519_pointCopy(uint64_t *output, uint64_t *input) {
    hc_MEMCPY(output, input, 5 * sizeof(*input));
    hc_MEMCPY(output + 5, input + 5, 5 * sizeof(*input));
}

static hc_ALWAYS_INLINE void curve25519_addanddoubleFmonty(uint64_t *pp, uint64_t *ppq, uint64_t *p, uint64_t *pq, uint64_t *qmqp) {
    uint64_t *qx = qmqp;
    uint64_t *x2 = pp;
    uint64_t *z2 = pp + 5;
    uint64_t *x3 = ppq;
    uint64_t *z3 = ppq + 5;
    uint64_t *x = p;
    uint64_t *z = p + 5;
    uint64_t *xprime = pq;
    uint64_t *zprime = pq + 5;
    uint64_t buf[40] = { 0 };
    uint64_t *origx = buf;
    uint64_t *origxprime0 = buf + 5;
    uint64_t *xxprime0;
    uint64_t *zzprime0;
    uint64_t *origxprime;
    xxprime0 = buf + 25;
    zzprime0 = buf + 30;
    hc_MEMCPY(origx, x, 5 * sizeof(*x));
    curve25519_fsum(x, z);
    curve25519_fdifference(z, origx);
    hc_MEMCPY(origxprime0, xprime, 5 * sizeof(*xprime));
    curve25519_fsum(xprime, zprime);
    curve25519_fdifference(zprime, origxprime0);
    curve25519_fmul(xxprime0, xprime, z);
    curve25519_fmul(zzprime0, x, zprime);
    origxprime = buf + 5;
    {
        uint64_t *xx0;
        uint64_t *zz0;
        uint64_t *xxprime;
        uint64_t *zzprime;
        uint64_t *zzzprime;
        xx0 = buf + 15;
        zz0 = buf + 20;
        xxprime = buf + 25;
        zzprime = buf + 30;
        zzzprime = buf + 35;
        hc_MEMCPY(origxprime, xxprime, 5 * sizeof(*xxprime));
        curve25519_fsum(xxprime, zzprime);
        curve25519_fdifference(zzprime, origxprime);
        curve25519_fsquareFsquareTimes(x3, xxprime, 1);
        curve25519_fsquareFsquareTimes(zzzprime, zzprime, 1);
        curve25519_fmul(z3, zzzprime, qx);
        curve25519_fsquareFsquareTimes(xx0, x, 1);
        curve25519_fsquareFsquareTimes(zz0, z, 1);
        {
            uint64_t *zzz;
            uint64_t *xx;
            uint64_t *zz;
            uint64_t scalar;
            zzz = buf + 10;
            xx = buf + 15;
            zz = buf + 20;
            curve25519_fmul(x2, xx, zz);
            curve25519_fdifference(zz, xx);
            scalar = 121665;
            curve25519_fscalar(zzz, zz, scalar);
            curve25519_fsum(zzz, xx);
            curve25519_fmul(z2, zzz, zz);
        }
    }
}

static hc_ALWAYS_INLINE void
curve25519_ladderSmallloopCmultSmallLoopStep(uint64_t *nq, uint64_t *nqpq, uint64_t *nq2, uint64_t *nqpq2, uint64_t *q, uint8_t byt) {
    uint64_t bit0 = (uint64_t)(byt >> 7);
    uint64_t bit;
    curve25519_pointSwapConditional(nq, nqpq, bit0);
    curve25519_addanddoubleFmonty(nq2, nqpq2, nq, nqpq, q);
    bit = (uint64_t)(byt >> 7);
    curve25519_pointSwapConditional(nq2, nqpq2, bit);
}

static hc_ALWAYS_INLINE void
curve25519_ladderSmallloopCmultSmallLoopDoubleStep(uint64_t *nq, uint64_t *nqpq, uint64_t *nq2, uint64_t *nqpq2, uint64_t *q, uint8_t byt) {
    uint8_t byt1;
    curve25519_ladderSmallloopCmultSmallLoopStep(nq, nqpq, nq2, nqpq2, q, byt);
    byt1 = (uint8_t)(byt << 1);
    curve25519_ladderSmallloopCmultSmallLoopStep(nq2, nqpq2, nq, nqpq, q, byt1);
}

static hc_ALWAYS_INLINE void
curve25519_ladderSmallloopCmultSmallLoop(uint64_t *nq, uint64_t *nqpq, uint64_t *nq2, uint64_t *nqpq2, uint64_t *q, uint8_t byt, uint32_t i) {
    while (i--) {
        curve25519_ladderSmallloopCmultSmallLoopDoubleStep(nq, nqpq, nq2, nqpq2, q, byt);
        byt <<= 2;
    }
}

static hc_ALWAYS_INLINE void
curve25519_ladderBigloopCmultBigLoop(uint8_t *n1, uint64_t *nq, uint64_t *nqpq, uint64_t *nq2, uint64_t *nqpq2, uint64_t *q, uint32_t i) {
    while (i--) {
        uint8_t byte = n1[i];
        curve25519_ladderSmallloopCmultSmallLoop(nq, nqpq, nq2, nqpq2, q, byte, 4);
    }
}

static void curve25519_ladderCmult(uint64_t *result, uint8_t *n1, uint64_t *q) {
    uint64_t point_buf[40] = { 0 };
    uint64_t *nq = point_buf;
    uint64_t *nqpq = point_buf + 10;
    uint64_t *nq2 = point_buf + 20;
    uint64_t *nqpq2 = point_buf + 30;
    curve25519_pointCopy(nqpq, q);
    nq[0] = 1;
    curve25519_ladderBigloopCmultBigLoop(n1, nq, nqpq, nq2, nqpq2, q, 32);
    curve25519_pointCopy(result, nq);
}

static hc_ALWAYS_INLINE void curve25519_formatFexpand(uint64_t *output, const uint8_t *input) {
    const uint8_t *x00 = input + 6;
    const uint8_t *x01 = input + 12;
    const uint8_t *x02 = input + 19;
    const uint8_t *x0 = input + 24;
    uint64_t i0, i1, i2, i3, i4, output0, output1, output2, output3, output4;
    i0 = curve25519_getUnalignedLe64(input);
    i1 = curve25519_getUnalignedLe64(x00);
    i2 = curve25519_getUnalignedLe64(x01);
    i3 = curve25519_getUnalignedLe64(x02);
    i4 = curve25519_getUnalignedLe64(x0);
    output0 = i0 & 0x7ffffffffffffLLU;
    output1 = i1 >> 3 & 0x7ffffffffffffLLU;
    output2 = i2 >> 6 & 0x7ffffffffffffLLU;
    output3 = i3 >> 1 & 0x7ffffffffffffLLU;
    output4 = i4 >> 12 & 0x7ffffffffffffLLU;
    output[0] = output0;
    output[1] = output1;
    output[2] = output2;
    output[3] = output3;
    output[4] = output4;
}

static hc_ALWAYS_INLINE void curve25519_formatFcontractFirstCarryPass(uint64_t *input) {
    uint64_t t0 = input[0];
    uint64_t t1 = input[1];
    uint64_t t2 = input[2];
    uint64_t t3 = input[3];
    uint64_t t4 = input[4];
    uint64_t t1_ = t1 + (t0 >> 51);
    uint64_t t0_ = t0 & 0x7ffffffffffffLLU;
    uint64_t t2_ = t2 + (t1_ >> 51);
    uint64_t t1__ = t1_ & 0x7ffffffffffffLLU;
    uint64_t t3_ = t3 + (t2_ >> 51);
    uint64_t t2__ = t2_ & 0x7ffffffffffffLLU;
    uint64_t t4_ = t4 + (t3_ >> 51);
    uint64_t t3__ = t3_ & 0x7ffffffffffffLLU;
    input[0] = t0_;
    input[1] = t1__;
    input[2] = t2__;
    input[3] = t3__;
    input[4] = t4_;
}

static hc_ALWAYS_INLINE void curve25519_formatFcontractFirstCarryFull(uint64_t *input) {
    curve25519_formatFcontractFirstCarryPass(input);
    curve25519_moduloCarryTop(input);
}

static hc_ALWAYS_INLINE void curve25519_formatFcontractSecondCarryPass(uint64_t *input) {
    uint64_t t0 = input[0];
    uint64_t t1 = input[1];
    uint64_t t2 = input[2];
    uint64_t t3 = input[3];
    uint64_t t4 = input[4];
    uint64_t t1_ = t1 + (t0 >> 51);
    uint64_t t0_ = t0 & 0x7ffffffffffffLLU;
    uint64_t t2_ = t2 + (t1_ >> 51);
    uint64_t t1__ = t1_ & 0x7ffffffffffffLLU;
    uint64_t t3_ = t3 + (t2_ >> 51);
    uint64_t t2__ = t2_ & 0x7ffffffffffffLLU;
    uint64_t t4_ = t4 + (t3_ >> 51);
    uint64_t t3__ = t3_ & 0x7ffffffffffffLLU;
    input[0] = t0_;
    input[1] = t1__;
    input[2] = t2__;
    input[3] = t3__;
    input[4] = t4_;
}

static hc_ALWAYS_INLINE void curve25519_formatFcontractSecondCarryFull(uint64_t *input) {
    uint64_t i0;
    uint64_t i1;
    uint64_t i0_;
    uint64_t i1_;
    curve25519_formatFcontractSecondCarryPass(input);
    curve25519_moduloCarryTop(input);
    i0 = input[0];
    i1 = input[1];
    i0_ = i0 & 0x7ffffffffffffLLU;
    i1_ = i1 + (i0 >> 51);
    input[0] = i0_;
    input[1] = i1_;
}

static hc_ALWAYS_INLINE void curve25519_formatFcontractTrim(uint64_t *input) {
    uint64_t a0 = input[0];
    uint64_t a1 = input[1];
    uint64_t a2 = input[2];
    uint64_t a3 = input[3];
    uint64_t a4 = input[4];
    uint64_t mask0 = curve25519_u64GteMask(a0, 0x7ffffffffffedLLU);
    uint64_t mask1 = curve25519_u64EqMask(a1, 0x7ffffffffffffLLU);
    uint64_t mask2 = curve25519_u64EqMask(a2, 0x7ffffffffffffLLU);
    uint64_t mask3 = curve25519_u64EqMask(a3, 0x7ffffffffffffLLU);
    uint64_t mask4 = curve25519_u64EqMask(a4, 0x7ffffffffffffLLU);
    uint64_t mask = (((mask0 & mask1) & mask2) & mask3) & mask4;
    uint64_t a0_ = a0 - (0x7ffffffffffedLLU & mask);
    uint64_t a1_ = a1 - (0x7ffffffffffffLLU & mask);
    uint64_t a2_ = a2 - (0x7ffffffffffffLLU & mask);
    uint64_t a3_ = a3 - (0x7ffffffffffffLLU & mask);
    uint64_t a4_ = a4 - (0x7ffffffffffffLLU & mask);
    input[0] = a0_;
    input[1] = a1_;
    input[2] = a2_;
    input[3] = a3_;
    input[4] = a4_;
}

static hc_ALWAYS_INLINE void curve25519_formatFcontractStore(uint8_t *output, uint64_t *input) {
    uint64_t t0 = input[0];
    uint64_t t1 = input[1];
    uint64_t t2 = input[2];
    uint64_t t3 = input[3];
    uint64_t t4 = input[4];
    uint64_t o0 = t1 << 51 | t0;
    uint64_t o1 = t2 << 38 | t1 >> 13;
    uint64_t o2 = t3 << 25 | t2 >> 26;
    uint64_t o3 = t4 << 12 | t3 >> 39;
    uint8_t *b0 = output;
    uint8_t *b1 = output + 8;
    uint8_t *b2 = output + 16;
    uint8_t *b3 = output + 24;
    curve25519_putUnalignedLe64(o0, b0);
    curve25519_putUnalignedLe64(o1, b1);
    curve25519_putUnalignedLe64(o2, b2);
    curve25519_putUnalignedLe64(o3, b3);
}

static hc_ALWAYS_INLINE void curve25519_formatFcontract(uint8_t *output, uint64_t *input) {
    curve25519_formatFcontractFirstCarryFull(input);
    curve25519_formatFcontractSecondCarryFull(input);
    curve25519_formatFcontractTrim(input);
    curve25519_formatFcontractStore(output, input);
}

static hc_ALWAYS_INLINE void curve25519_formatScalarOfPoint(uint8_t *scalar, uint64_t *point) {
    uint64_t *x = point;
    uint64_t *z = point + 5;
    uint64_t buf[10] hc_ALIGNED(32) = { 0 };
    uint64_t *zmone = buf;
    uint64_t *sc = buf + 5;
    curve25519_crecip(zmone, z);
    curve25519_fmul(sc, x, zmone);
    curve25519_formatFcontract(scalar, sc);
}

static void curve25519(uint8_t *public, const uint8_t *secret, const uint8_t *basepoint) {
    uint64_t buf0[10] hc_ALIGNED(32) = { 0 };
    uint64_t *x0 = buf0;
    uint64_t *z = buf0 + 5;
    uint64_t *q;
    curve25519_formatFexpand(x0, basepoint);
    z[0] = 1;
    q = buf0;
    {
        uint8_t e[32] hc_ALIGNED(32) = { 0 };
        uint8_t *scalar;
        hc_MEMCPY(e, secret, 32);
        curve25519_clampSecret(e);
        scalar = e;
        {
            uint64_t buf[15] = { 0 };
            uint64_t *nq = buf;
            uint64_t *x = nq;
            x[0] = 1;
            curve25519_ladderCmult(nq, scalar, q);
            curve25519_formatScalarOfPoint(mypublic, nq);
            curve25519_memzeroExplicit(buf, sizeof(buf));
        }
        curve25519_memzeroExplicit(e, sizeof(e));
    }
    curve25519_memzeroExplicit(buf0, sizeof(buf0));
}

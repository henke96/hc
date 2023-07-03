// Public domain by Adam Langley <agl@imperialviolet.org> (curve25519-donna)
// Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>

// Find the difference of two numbers: output = in - output
// Assumes that out[i] < 2**52
// On return, out[i] < 2**55
static hc_INLINE void _x25519_differenceBackwards(uint64_t *out, const uint64_t *in) {
    out[0] = in[0] + (uint64_t)0x3FFFFFFFFFFF68 - out[0];
    out[1] = in[1] + (uint64_t)0x3FFFFFFFFFFFF8 - out[1];
    out[2] = in[2] + (uint64_t)0x3FFFFFFFFFFFF8 - out[2];
    out[3] = in[3] + (uint64_t)0x3FFFFFFFFFFFF8 - out[3];
    out[4] = in[4] + (uint64_t)0x3FFFFFFFFFFFF8 - out[4];
}

// Input: Q, Q', Q-Q'
// Output: 2Q, Q+Q'
//
// x2 z3: long form
// x3 z3: long form
// x z: short form, destroyed
// xprime zprime: short form, destroyed
// qmqp: short form, preserved
static void _x25519_monty(
    uint64_t *x2, uint64_t *z2, // Output 2Q
    uint64_t *x3, uint64_t *z3, // Output Q + Q'
    uint64_t *x, uint64_t *z,   // Input Q
    uint64_t *xprime, uint64_t *zprime, // Input Q'
    const uint64_t *qmqp // Input Q - Q'
) {
    uint64_t origx[5], origxprime[5], zzz[5], xx[5], zz[5], xxprime[5], zzprime[5], zzzprime[5];

    hc_MEMCPY(&origx[0], x, 5 * sizeof(uint64_t));
    curve25519_addNoReduce(x, x, z);
    _x25519_differenceBackwards(z, &origx[0]); // Does x - z

    hc_MEMCPY(&origxprime[0], xprime, sizeof(uint64_t) * 5);
    curve25519_addNoReduce(xprime, xprime, zprime);
    _x25519_differenceBackwards(zprime, &origxprime[0]);
    curve25519_mul(&xxprime[0], xprime, z);
    curve25519_mul(&zzprime[0], x, zprime);
    hc_MEMCPY(&origxprime[0], &xxprime[0], sizeof(uint64_t) * 5);
    curve25519_addNoReduce(&xxprime[0], &xxprime[0], &zzprime[0]);
    _x25519_differenceBackwards(&zzprime[0], &origxprime[0]);
    curve25519_squareN(x3, &xxprime[0], 1);
    curve25519_squareN(&zzzprime[0], &zzprime[0], 1);
    curve25519_mul(z3, &zzzprime[0], qmqp);

    curve25519_squareN(&xx[0], x, 1);
    curve25519_squareN(zz, z, 1);
    curve25519_mul(x2, &xx[0], zz);
    _x25519_differenceBackwards(zz, &xx[0]); // Does zz = xx - zz
    curve25519_mulScalar(&zzz[0], zz, 121665);
    curve25519_addNoReduce(&zzz[0], &zzz[0], &xx[0]);
    curve25519_mul(z2, zz, &zzz[0]);
}

// Calculates nQ where Q is the x-coordinate of a point on the curve.
//
// resultx/resultz: the x coordinate of the resulting curve point (short form).
// n: a little endian, 32-byte number.
// q: a point of the curve (short form).
static void _x25519_cmult(uint64_t *resultx, uint64_t *resultz, const uint8_t *n, const uint64_t *q) {
    uint64_t a[5] = {0}, b[5] = {1}, c[5] = {1}, d[5] = {0};
    uint64_t *nqpqx = &a[0], *nqpqz = &b[0], *nqx = &c[0], *nqz = &d[0];
    uint64_t e[5] = {0}, f[5] = {1}, g[5] = {0}, h[5] = {1};
    uint64_t *nqpqx2 = &e[0], *nqpqz2 = &f[0], *nqx2 = &g[0], *nqz2 = &h[0];

    hc_MEMCPY(nqpqx, q, sizeof(uint64_t) * 5);

    for (int32_t i = 0; i < 32; ++i) {
        uint8_t byte = n[31 - i];
        for (int32_t j = 0; j < 8; ++j) {
            const uint64_t bit = byte >> 7;

            curve25519_cSwap(nqx, nqpqx, bit);
            curve25519_cSwap(nqz, nqpqz, bit);
            _x25519_monty(
                nqx2, nqz2,
                nqpqx2, nqpqz2,
                nqx, nqz,
                nqpqx, nqpqz,
                q
            );
            curve25519_cSwap(nqx2, nqpqx2, bit);
            curve25519_cSwap(nqz2, nqpqz2, bit);

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

static const uint8_t x25519_ecdhBasepoint[32] = { 9 };

// All arguments are 32 bytes.
static void x25519(const uint8_t *secret, const uint8_t *public, uint8_t *out) {
    uint64_t bp[5], x[5], z[5], zmone[5];
    uint8_t e[32];

    hc_MEMCPY(&e[0], &secret[0], 32);
    e[0] &= 248;
    e[31] &= 127;
    e[31] |= 64;

    curve25519_load(bp, public);
    _x25519_cmult(x, z, e, bp);
    curve25519_recip(zmone, z);
    curve25519_mul(z, x, zmone);
    curve25519_store(out, z);
}

// Public domain by Adam Langley <agl@imperialviolet.org> (curve25519-donna)
// Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>

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
    curve25519_subNoReduce(&z[0], &origx[0], &z[0]);

    hc_MEMCPY(&origxprime[0], xprime, sizeof(uint64_t) * 5);
    curve25519_addNoReduce(xprime, xprime, zprime);
    curve25519_subNoReduce(&zprime[0], &origxprime[0], &zprime[0]);
    curve25519_mul(&xxprime[0], xprime, z);
    curve25519_mul(&zzprime[0], x, zprime);
    hc_MEMCPY(&origxprime[0], &xxprime[0], sizeof(uint64_t) * 5);
    curve25519_addNoReduce(&xxprime[0], &xxprime[0], &zzprime[0]);
    curve25519_subNoReduce(&zzprime[0], &origxprime[0], &zzprime[0]);
    curve25519_squareN(x3, &xxprime[0], 1);
    curve25519_squareN(&zzzprime[0], &zzprime[0], 1);
    curve25519_mul(z3, &zzzprime[0], qmqp);

    curve25519_squareN(&xx[0], x, 1);
    curve25519_squareN(zz, z, 1);
    curve25519_mul(x2, &xx[0], zz);
    curve25519_subNoReduce(&zz[0], &xx[0], &zz[0]);
    curve25519_mulScalar(&zzz[0], zz, 121665);
    curve25519_addNoReduce(&zzz[0], &zzz[0], &xx[0]);
    curve25519_mul(z2, zz, &zzz[0]);
}

// Calculates nQ where Q is the x-coordinate of a point on the curve.
//
// outX/outZ: The x coordinate of the resulting curve point (short form).
// n: A 32 byte number.
// q: A point of the curve (short form).
static void _x25519_mul(uint64_t *outX, uint64_t *outZ, const uint8_t *n, const uint64_t *q) {
    uint64_t a[5], b[5] = {1}, c[5] = {1}, d[5] = {0};
    hc_MEMCPY(&a[0], q, sizeof(uint64_t) * 5);
    uint64_t *nqpqx = &a[0], *nqpqz = &b[0], *nqx = &c[0], *nqz = &d[0];

    uint64_t e[5] = {0}, f[5] = {1}, g[5] = {0}, h[5] = {1};
    uint64_t *nqpqx2 = &e[0], *nqpqz2 = &f[0], *nqx2 = &g[0], *nqz2 = &h[0];

    int32_t i = 256;
    while (i) {
        --i;
        const uint64_t bit = (n[i >> 3] >> (i & 7)) & 1;

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
    }

    hc_MEMCPY(outX, nqx, sizeof(uint64_t) * 5);
    hc_MEMCPY(outZ, nqz, sizeof(uint64_t) * 5);
}

static const uint8_t x25519_ecdhBasepoint[32] = { 9 };

// All arguments are 32 bytes.
static void x25519(const uint8_t *secret, const uint8_t *public, uint8_t *out) {
    uint64_t bp[5], x[5], z[5];
    uint8_t e[32];

    hc_MEMCPY(&e[0], &secret[0], 32);
    e[0] &= 248;
    e[31] &= 127;
    e[31] |= 64;

    curve25519_load(&bp[0], public);
    _x25519_mul(&x[0], &z[0], &e[0], &bp[0]);
    curve25519_invert(&z[0], &z[0]);
    curve25519_mul(&z[0], &x[0], &z[0]);
    curve25519_reduce(&z[0]);
    curve25519_store(out, &z[0]);
}

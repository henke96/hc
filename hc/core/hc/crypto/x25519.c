// Public domain by Adam Langley <agl@imperialviolet.org> (curve25519-donna)
// Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>

// Calculates nQ where Q is the x-coordinate of a point on the curve.
//
// outX/outZ: The x coordinate of the resulting curve point (short form).
// n: A 32 byte number.
// q: A point of the curve (short form).
static void _x25519_mul(uint64_t *outX, uint64_t *outZ, const uint8_t *n, const uint64_t *q) {
    uint64_t x0[5] = {1}, z0[5] = {0};
    uint64_t x1[5], z1[5] = {1};
    hc_MEMCPY(&x1[0], q, sizeof(uint64_t) * 5);

    uint64_t swap = 0;
    int32_t i = 256;
    while (i) {
        --i;

        const uint64_t bit = (n[i >> 3] >> (i & 7)) & 1;
        swap = swap ^ bit;
        curve25519_cSwap(&x0[0], &x1[0], swap);
        curve25519_cSwap(&z0[0], &z1[0], swap);
        swap = bit;

        uint64_t a[5], b[5];
        curve25519_addNoReduce(&a[0], &x0[0], &z0[0]);
        curve25519_subNoReduce(&b[0], &x1[0], &z1[0]);
        curve25519_mul(&b[0], &a[0], &b[0]);
        curve25519_squareN(&a[0], &a[0], 1);

        curve25519_subNoReduce(&x0[0], &x0[0], &z0[0]);
        curve25519_addNoReduce(&z1[0], &x1[0], &z1[0]);
        curve25519_mul(&z1[0], &z1[0], &x0[0]);
        curve25519_squareN(&x0[0], &x0[0], 1);

        curve25519_addNoReduce(&x1[0], &z1[0], &b[0]);
        curve25519_squareN(&x1[0], &x1[0], 1);

        curve25519_subNoReduce(&z1[0], &z1[0], &b[0]);
        curve25519_squareN(&z1[0], &z1[0], 1);
        curve25519_mul(&z1[0], &z1[0], q);

        curve25519_subNoReduce(&b[0], &a[0], &x0[0]);
        curve25519_mul(&x0[0], &a[0], &x0[0]);

        curve25519_mulScalar(&z0[0], &b[0], 121665);
        curve25519_addNoReduce(&z0[0], &z0[0], &a[0]);
        curve25519_mul(&z0[0], &b[0], &z0[0]);
    }
    curve25519_cSwap(&x0[0], &x1[0], swap);
    curve25519_cSwap(&z0[0], &z1[0], swap);

    hc_MEMCPY(outX, &x0[0], sizeof(uint64_t) * 5);
    hc_MEMCPY(outZ, &z0[0], sizeof(uint64_t) * 5);
}

static const uint8_t x25519_ecdhBasepoint[32] = { 9 };

// All arguments are 32 bytes.
static void x25519(void *out, const void *secret, const void *public) {
    uint64_t bp[5], x[5], z[5];
    uint8_t e[32];

    hc_MEMCPY(&e[0], secret, 32);
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

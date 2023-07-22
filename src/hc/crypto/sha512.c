// Based on code by D. J. Bernstein, public domain.

#define sha512_HASH_SIZE 64

#define _sha512_BLOCK_SIZE 128
#define _sha512_BLOCK_SHIFT 7

struct sha512 {
    uint64_t state[8];
    uint64_t blockCounter;
    uint8_t buffer[_sha512_BLOCK_SIZE];
    int64_t bufferSize;
};

#define _sha512_SIGMA0_A(X) (hc_ROTR64(X, 1) ^ hc_ROTR64(X, 8) ^ ((X) >> 7))
#define _sha512_SIGMA1_A(X) (hc_ROTR64(X, 19) ^ hc_ROTR64(X, 61) ^ ((X) >> 6))

#define _sha512_CH(X, Y, Z) (Z ^ (X & (Y ^ Z)))
#define _sha512_MAJ(X, Y, Z) ((X & (Y ^ Z)) ^ (Y & Z))
#define _sha512_SIGMA0_B(X) (hc_ROTR64(X, 28) ^ hc_ROTR64(X, 34) ^ hc_ROTR64(X, 39))
#define _sha512_SIGMA1_B(X) (hc_ROTR64(X, 14) ^ hc_ROTR64(X, 18) ^ hc_ROTR64(X, 41))

static const uint64_t _sha512_round[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

static void _sha512_blocks(uint64_t *state, const void *in, int64_t numBlocks) {
    uint64_t w[80];
    uint64_t r[88];

    do {
        for (int32_t i = 0; i < 8; ++i) r[7 - i] = state[i];
        for (int32_t i = 0; i < 16; ++i) w[i] = mem_loadU64BE(&in[i << 3]);

        for (int32_t i = 0;; ++i) {
            uint64_t x = r[i] + w[i] + _sha512_round[i] + _sha512_SIGMA1_B(r[i + 3]) + _sha512_CH(r[i + 3], r[i + 2], r[i + 1]);
            r[i + 4] += x;
            r[i + 8] = x + _sha512_SIGMA0_B(r[i + 7]) + _sha512_MAJ(r[i + 7], r[i + 6], r[i + 5]);

            if (i < 64) w[i + 16] = w[i] + w[i + 9] + _sha512_SIGMA1_A(w[i + 14]) + _sha512_SIGMA0_A(w[i + 1]);
            else if (i == 79) break;
        }

        for (int32_t i = 0; i < 8; ++i) state[i] += r[87 - i];

        in += _sha512_BLOCK_SIZE;
    } while (--numBlocks);
}

hc_UNUSED
static void sha512_init(struct sha512 *self) {
    self->state[0] = 0x6a09e667f3bcc908;
    self->state[1] = 0xbb67ae8584caa73b;
    self->state[2] = 0x3c6ef372fe94f82b;
    self->state[3] = 0xa54ff53a5f1d36f1;
    self->state[4] = 0x510e527fade682d1;
    self->state[5] = 0x9b05688c2b3e6c1f;
    self->state[6] = 0x1f83d9abfb41bd6b;
    self->state[7] = 0x5be0cd19137e2179;
    self->blockCounter = 0;
    self->bufferSize = 0;
}

hc_UNUSED
static void sha512_init384(struct sha512 *self) {
    self->state[0] = 0xcbbb9d5dc1059ed8;
    self->state[1] = 0x629a292a367cd507;
    self->state[2] = 0x9159015a3070dd17;
    self->state[3] = 0x152fecd8f70e5939;
    self->state[4] = 0x67332667ffc00b31;
    self->state[5] = 0x8eb44a8768581511;
    self->state[6] = 0xdb0c2e0d64f98fa7;
    self->state[7] = 0x47b5481dbefa4fa4;
    self->blockCounter = 0;
    self->bufferSize = 0;
}

static void sha512_update(struct sha512 *self, const void *in, int64_t size) {
    if (self->bufferSize > 0) {
        int64_t numToRead = _sha512_BLOCK_SIZE - self->bufferSize;
        if (numToRead > size) numToRead = size;
        hc_MEMCPY(&self->buffer[self->bufferSize], in, (size_t)numToRead);
        self->bufferSize += numToRead;
        if (self->bufferSize < _sha512_BLOCK_SIZE) return;
        _sha512_blocks(&self->state[0], &self->buffer[0], 1);
        ++self->blockCounter;
        in += numToRead;
        size -= numToRead;
    }
    int64_t numBlocks = size >> _sha512_BLOCK_SHIFT;
    if (numBlocks > 0) {
        _sha512_blocks(&self->state[0], in, numBlocks);
        self->blockCounter += (uint64_t)numBlocks;
    }

    self->bufferSize = math_ALIGN_REMAINDER(size, _sha512_BLOCK_SIZE);
    if (self->bufferSize > 0) {
        in += math_ALIGN_BACKWARD(size, _sha512_BLOCK_SIZE);
        hc_MEMCPY(&self->buffer[0], in, (size_t)self->bufferSize);
    }
}

static void sha512_finish(struct sha512 *self, void *hash) {
    union {
        uint64_t u64[2];
        uint128_t u128;
    } numBits;
    numBits.u128 = (((uint128_t)self->blockCounter << _sha512_BLOCK_SHIFT) + (uint128_t)self->bufferSize) << 3;

    self->buffer[self->bufferSize++] = 0x80;
    if (self->bufferSize > 112) {
        hc_MEMSET(&self->buffer[self->bufferSize], 0, (size_t)(128 - self->bufferSize));
        _sha512_blocks(&self->state[0], &self->buffer[0], 1);
        self->bufferSize = 0;
    }
    hc_MEMSET(&self->buffer[self->bufferSize], 0, (size_t)(112 - self->bufferSize));
    mem_storeU64BE(&self->buffer[112], numBits.u64[1]);
    mem_storeU64BE(&self->buffer[120], numBits.u64[0]);
    _sha512_blocks(&self->state[0], &self->buffer[0], 1);
    for (int32_t i = 0; i < 8; ++i) mem_storeU64BE(hash + (i << 3), self->state[i]);
}

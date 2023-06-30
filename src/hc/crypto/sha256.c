// Based on code by D. J. Bernstein, public domain.

#define sha256_HASH_SIZE 32
#define _sha256_BLOCK_SHIFT 6
#define _sha256_BLOCK_MASK 63
#define _sha256_BLOCK_SIZE 64

struct sha256 {
    uint32_t state[8];
    uint64_t blockCounter;
    uint8_t buffer[_sha256_BLOCK_SIZE];
    int64_t bufferSize;
};

static hc_INLINE uint32_t _sha256_loadU32BE(const uint8_t *in) {
    uint32_t x;
    hc_MEMCPY(&x, in, 4);
    return hc_BSWAP32(x);
}

static hc_INLINE void _sha256_storeU32BE(uint8_t *out, uint32_t in) {
    in = hc_BSWAP32(in);
    hc_MEMCPY(out, &in, 4);
}

static hc_INLINE void _sha256_storeU64BE(uint8_t *out, uint64_t in) {
    in = hc_BSWAP64(in);
    hc_MEMCPY(out, &in, 8);
}

#define _sha256_SIGMA0_A(X) (hc_ROTR32(X, 7) ^ hc_ROTR32(X, 18) ^ ((X) >> 3))
#define _sha256_SIGMA1_A(X) (hc_ROTR32(X, 17) ^ hc_ROTR32(X, 19) ^ ((X) >> 10))

#define _sha256_CH(X, Y, Z) (Z ^ (X & (Y ^ Z)))
#define _sha256_MAJ(X, Y, Z) ((X & (Y ^ Z)) ^ (Y & Z))
#define _sha256_SIGMA0_B(X) (hc_ROTR32(X, 2) ^ hc_ROTR32(X, 13) ^ hc_ROTR32(X, 22))
#define _sha256_SIGMA1_B(X) (hc_ROTR32(X, 6) ^ hc_ROTR32(X, 11) ^ hc_ROTR32(X, 25))

static const uint64_t _sha256_round[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void _sha256_blocks(uint32_t *state, const uint8_t *in, int64_t numBlocks) {
    uint32_t w[64];
    uint32_t r[72];

    for (int32_t i = 0; i < 8; ++i) r[7 - i] = state[i];

    for (; numBlocks > 0; --numBlocks) {
        for (int32_t i = 0; i < 16; ++i) w[i] = _sha256_loadU32BE(&in[4 * i]);

        for (int32_t i = 0;; ++i) {
            uint32_t y = w[i];
            uint32_t x = r[i];
            x += y;
            x += _sha256_round[i];
            x += _sha256_SIGMA1_B(r[i + 3]);
            x += _sha256_CH(r[i + 3], r[i + 2], r[i + 1]);
            r[i + 4] += x;
            x += _sha256_SIGMA0_B(r[i + 7]);
            x += _sha256_MAJ(r[i + 7], r[i + 6], r[i + 5]);
            r[i + 8] = x;

            if (i < 48) {
                y += w[i + 9];
                y += _sha256_SIGMA1_A(w[i + 14]);
                y += _sha256_SIGMA0_A(w[i + 1]);
                w[i + 16] = y;
            } else if (i == 63) break;
        }

        for (int32_t i = 0; i < 8; ++i) {
            uint32_t x = r[71 - i] + state[i];
            state[i] = r[7 - i] = x;
        }
        in += _sha256_BLOCK_SIZE;
    }
}

static void sha256_init(struct sha256 *self) {
    self->state[0] = 0x6a09e667;
    self->state[1] = 0xbb67ae85;
    self->state[2] = 0x3c6ef372;
    self->state[3] = 0xa54ff53a;
    self->state[4] = 0x510e527f;
    self->state[5] = 0x9b05688c;
    self->state[6] = 0x1f83d9ab;
    self->state[7] = 0x5be0cd19;
    self->blockCounter = 0;
    self->bufferSize = 0;
}

static void sha256_update(struct sha256 *self, const uint8_t *in, int64_t size) {
    if (self->bufferSize > 0) {
        int64_t numToRead = _sha256_BLOCK_SIZE - self->bufferSize;
        if (numToRead > size) numToRead = size;
        for (int64_t i = 0; i < numToRead; ++i) self->buffer[self->bufferSize + i] = in[i];
        self->bufferSize += numToRead;
        if (self->bufferSize < _sha256_BLOCK_SIZE) return;
        _sha256_blocks(&self->state[0], &self->buffer[0], 1);
        ++self->blockCounter;
        in += numToRead;
        size -= numToRead;
    }
    int64_t numBlocks = size >> _sha256_BLOCK_SHIFT;
    _sha256_blocks(&self->state[0], in, numBlocks);
    self->blockCounter += (uint64_t)numBlocks;

    self->bufferSize = size & _sha256_BLOCK_MASK;
    if (self->bufferSize > 0) {
        in += size & ~(int64_t)_sha256_BLOCK_MASK;
        for (int64_t i = 0; i < self->bufferSize; ++i) self->buffer[i] = in[i];
    }
}

static void sha256_finish(struct sha256 *self, uint8_t *hash) {
    uint64_t numBits = ((self->blockCounter << _sha256_BLOCK_SHIFT) + (uint64_t)self->bufferSize) << 3;

    self->buffer[self->bufferSize++] = 0x80;
    if (self->bufferSize > 56) {
        hc_MEMSET(&self->buffer[self->bufferSize], 0, (uint64_t)(64 - self->bufferSize));
        _sha256_blocks(&self->state[0], &self->buffer[0], 1);
        self->bufferSize = 0;
    }
    hc_MEMSET(&self->buffer[self->bufferSize], 0, (uint64_t)(56 - self->bufferSize));
    _sha256_storeU64BE(&self->buffer[56], numBits);
    _sha256_blocks(&self->state[0], &self->buffer[0], 1);
    for (int32_t i = 0; i < 8; ++i) _sha256_storeU32BE(&hash[i * 4], self->state[i]);
}

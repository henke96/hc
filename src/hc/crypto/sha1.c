#define sha1_HASH_SIZE 20

#define _sha1_BLOCK_SIZE 64
#define _sha1_BLOCK_SHIFT 6

struct sha1 {
    uint64_t blockCounter;
    uint8_t buffer[_sha1_BLOCK_SIZE];
    ssize_t bufferSize;
    uint32_t state[5];
    hc_LP64_PAD(__pad)
};

static void _sha1_blocks(uint32_t *state, const void *in, ssize_t numBlocks) {
    uint32_t w[80];
    uint32_t r[85];

    do {
        r[0] = state[4];
        r[1] = state[3];
        r[2] = state[2];
        r[3] = state[1];
        r[4] = state[0];
        for (int32_t i = 0; i < 16; ++i) w[i] = mem_loadU32BE(&in[i << 2]);
        for (int32_t i = 0; i < 64; ++i) w[i + 16] = hc_ROTL32(w[i + 13] ^ w[i + 8] ^ w[i + 2] ^ w[i], 1);

        int32_t i = 0;
        for (; i < 20; ++i) {
            r[i + 5] = r[i] + w[i] + 0x5A827999 + hc_ROTL32(r[i + 4], 5) + ((r[i + 3] & (r[i + 2] ^ r[i + 1])) ^ r[i + 1]);
            r[i + 3] = hc_ROTL32(r[i + 3], 30);
        }
        for (; i < 40; ++i) {
            r[i + 5] = r[i] + w[i] + 0x6ED9EBA1 + hc_ROTL32(r[i + 4], 5) + (r[i + 3] ^ r[i + 2] ^ r[i + 1]);
            r[i + 3] = hc_ROTL32(r[i + 3], 30);
        }
        for (; i < 60; ++i) {
            r[i + 5] = r[i] + w[i] + 0x8F1BBCDC + hc_ROTL32(r[i + 4], 5) + (((r[i + 3] | r[i + 2]) & r[i + 1]) | (r[i + 3] & r[i + 2]));
            r[i + 3] = hc_ROTL32(r[i + 3], 30);
        }
        for (; i < 80; ++i) {
            r[i + 5] = r[i] + w[i] + 0xCA62C1D6 + hc_ROTL32(r[i + 4], 5) + (r[i + 3] ^ r[i + 2] ^ r[i + 1]);
            r[i + 3] = hc_ROTL32(r[i + 3], 30);
        }

        state[0] += r[84];
        state[1] += r[83];
        state[2] += r[82];
        state[3] += r[81];
        state[4] += r[80];

        in += _sha1_BLOCK_SIZE;
    } while (--numBlocks);
}

static void sha1_init(struct sha1 *self) {
    self->state[0] = 0x67452301;
    self->state[1] = 0xEFCDAB89;
    self->state[2] = 0x98BADCFE;
    self->state[3] = 0x10325476;
    self->state[4] = 0xC3D2E1F0;
    self->blockCounter = 0;
    self->bufferSize = 0;
}

static void sha1_update(struct sha1 *self, const void *in, ssize_t size) {
    if (self->bufferSize > 0) {
        ssize_t numToRead = _sha1_BLOCK_SIZE - self->bufferSize;
        if (numToRead > size) numToRead = size;
        hc_MEMCPY(&self->buffer[self->bufferSize], in, (size_t)numToRead);
        self->bufferSize += numToRead;
        if (self->bufferSize < _sha1_BLOCK_SIZE) return;
        _sha1_blocks(&self->state[0], &self->buffer[0], 1);
        ++self->blockCounter;
        in += numToRead;
        size -= numToRead;
    }
    ssize_t numBlocks = size >> _sha1_BLOCK_SHIFT;
    if (numBlocks > 0) {
        _sha1_blocks(&self->state[0], in, numBlocks);
        self->blockCounter += (uint64_t)numBlocks;
    }

    self->bufferSize = math_ALIGN_REMAINDER(size, _sha1_BLOCK_SIZE);
    if (self->bufferSize > 0) {
        in += math_ALIGN_BACKWARD(size, _sha1_BLOCK_SIZE);
        hc_MEMCPY(&self->buffer[0], in, (size_t)self->bufferSize);
    }
}

static void sha1_finish(struct sha1 *self, void *hash) {
    uint64_t numBits = ((self->blockCounter << _sha1_BLOCK_SHIFT) + (uint64_t)self->bufferSize) << 3;

    self->buffer[self->bufferSize++] = 0x80;
    if (self->bufferSize > 56) {
        hc_MEMSET(&self->buffer[self->bufferSize], 0, (size_t)(64 - self->bufferSize));
        _sha1_blocks(&self->state[0], &self->buffer[0], 1);
        self->bufferSize = 0;
    }
    hc_MEMSET(&self->buffer[self->bufferSize], 0, (size_t)(56 - self->bufferSize));
    mem_storeU64BE(&self->buffer[56], numBits);
    _sha1_blocks(&self->state[0], &self->buffer[0], 1);
    for (int32_t i = 0; i < 5; ++i) mem_storeU32BE(hash + (i << 2), self->state[i]);
}

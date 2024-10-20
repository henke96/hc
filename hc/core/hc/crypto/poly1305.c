// Andrew Moon's poly1305-donna, public domain.

#define poly1305_MAC_SIZE 16

#define _poly1305_BLOCK_SIZE 16
#define _poly1305_BLOCK_SHIFT 4

struct poly1305 {
    uint64_t r[3];
    uint64_t h[3];
    uint64_t pad[2];
    ssize_t bufferSize;
    hc_PTR32_PAD(__pad)
    uint8_t buffer[_poly1305_BLOCK_SIZE];
};

static void _poly1305_blocks(struct poly1305 *self, const uint8_t *in, ssize_t numBlocks, uint64_t hibit) {
    uint64_t r0 = self->r[0];
    uint64_t r1 = self->r[1];
    uint64_t r2 = self->r[2];

    uint64_t h0 = self->h[0];
    uint64_t h1 = self->h[1];
    uint64_t h2 = self->h[2];

    uint64_t s1 = r1 * 20;
    uint64_t s2 = r2 * 20;

    do {
        // h += m[i]
        uint64_t t0 = mem_loadU64(&in[0]);
        uint64_t t1 = mem_loadU64(&in[8]);

        h0 += t0 & 0xFFFFFFFFFFF;
        h1 += ((t0 >> 44) | (t1 << 20)) & 0xFFFFFFFFFFF;
        h2 += ((t1 >> 24) & 0x3FFFFFFFFFF) | hibit;

        // h *= r
        uint128_t d0 = hc_MUL128_64x64(h0, r0) + hc_MUL128_64x64(h1, s2) + hc_MUL128_64x64(h2, s1);
        uint128_t d1 = hc_MUL128_64x64(h0, r1) + hc_MUL128_64x64(h1, r0) + hc_MUL128_64x64(h2, s2);
        uint128_t d2 = hc_MUL128_64x64(h0, r2) + hc_MUL128_64x64(h1, r1) + hc_MUL128_64x64(h2, r0);

        // (partial) h %= p
        h0 = (uint64_t)d0 & 0xFFFFFFFFFFF;
        d1 += (uint64_t)(d0 >> 44);
        h1 = (uint64_t)d1 & 0xFFFFFFFFFFF;
        d2 += (uint64_t)(d1 >> 44);
        h2 = (uint64_t)d2 & 0x3FFFFFFFFFF;
        h0 += (uint64_t)(d2 >> 42) * 5;
        h1 += (h0 >> 44);
        h0 &= 0xFFFFFFFFFFF;

        in += _poly1305_BLOCK_SIZE;
    } while (--numBlocks);

    self->h[0] = h0;
    self->h[1] = h1;
    self->h[2] = h2;
}

// `key` is 32 bytes.
static void poly1305_init(struct poly1305 *self, const uint8_t *key) {
    self->bufferSize = 0;

    // r &= 0xFFFFFFC0FFFFFFC0FFFFFFC0FFFFFFF
    uint64_t t0 = mem_loadU64(&key[0]);
    uint64_t t1 = mem_loadU64(&key[8]);

    self->r[0] = t0 & 0xFFC0FFFFFFF;
    self->r[1] = ((t0 >> 44) | (t1 << 20)) & 0xFFFFFC0FFFF;
    self->r[2] = (t1 >> 24) & 0x00FFFFFFC0F;

    // h = 0
    self->h[0] = 0;
    self->h[1] = 0;
    self->h[2] = 0;

    // Save pad for later.
    hc_MEMCPY(&self->pad[0], &key[16], 8);
    hc_MEMCPY(&self->pad[1], &key[24], 8);
}

static void poly1305_update(struct poly1305 *self, const void *in, ssize_t size) {
    if (self->bufferSize > 0) {
        ssize_t numToRead = _poly1305_BLOCK_SIZE - self->bufferSize;
        if (numToRead > size) numToRead = size;
        hc_MEMCPY(&self->buffer[self->bufferSize], in, (size_t)numToRead);
        self->bufferSize += numToRead;
        if (self->bufferSize < _poly1305_BLOCK_SIZE) return;
        _poly1305_blocks(self, &self->buffer[0], 1, 0x10000000000);
        in += numToRead;
        size -= numToRead;
    }

    ssize_t numBlocks = size >> _poly1305_BLOCK_SHIFT;
    if (numBlocks > 0) _poly1305_blocks(self, in, numBlocks, 0x10000000000);

    self->bufferSize = math_ALIGN_REMAINDER(size, _poly1305_BLOCK_SIZE);
    if (self->bufferSize > 0) {
        in += math_ALIGN_BACKWARD(size, _poly1305_BLOCK_SIZE);
        hc_MEMCPY(&self->buffer[0], in, (size_t)self->bufferSize);
    }
}

static void poly1305_finish(struct poly1305 *self, void *mac) {
    if (self->bufferSize > 0) {
        ssize_t i = self->bufferSize;
        self->buffer[i++] = 1;
        hc_MEMSET(&self->buffer[i], 0, (size_t)(_poly1305_BLOCK_SIZE - i));
        _poly1305_blocks(self, &self->buffer[0], 1, 0);
    }

    // Fully carry h
    uint64_t h0 = self->h[0];
    uint64_t h1 = self->h[1];
    uint64_t h2 = self->h[2];

    h2 += (h1 >> 44);
    h1 &= 0xFFFFFFFFFFF;
    h0 += (h2 >> 42) * 5;
    h2 &= 0x3FFFFFFFFFF;
    h1 += (h0 >> 44);
    h0 &= 0xFFFFFFFFFFF;
    h2 += (h1 >> 44);
    h1 &= 0xFFFFFFFFFFF;
    h0 += (h2 >> 42) * 5;
    h2 &= 0x3FFFFFFFFFF;
    h1 += (h0 >> 44);
    h0 &= 0xFFFFFFFFFFF;

    // Compute h + -p
    uint64_t g0 = h0 + 5;
    uint64_t g1 = h1 + (g0 >> 44);
    uint64_t g2 = h2 + (g1 >> 44) - 0x80000000000;
    g0 &= 0xFFFFFFFFFFF;
    g1 &= 0xFFFFFFFFFFF;

    // Select h if h < p, or h + -p if h >= p
    uint64_t c = (g2 >> 63) - 1;
    g0 &= c;
    g1 &= c;
    g2 &= c;
    c = ~c;
    h0 = (h0 & c) | g0;
    h1 = (h1 & c) | g1;
    h2 = (h2 & c) | g2;

    // h = (h + pad)
    uint64_t t0 = self->pad[0];
    uint64_t t1 = self->pad[1];

    h0 += t0 & 0xFFFFFFFFFFF;
    h1 += (((t0 >> 44) | (t1 << 20)) & 0xFFFFFFFFFFF) + (h0 >> 44);
    h2 += ((t1 >> 24) & 0x3FFFFFFFFFF) + (h1 >> 44);
    h0 &= 0xFFFFFFFFFFF;
    h1 &= 0xFFFFFFFFFFF;
    h2 &= 0x3FFFFFFFFFF;

    // mac = h % (2^128)
    h0 = h0 | (h1 << 44);
    h1 = (h1 >> 20) | (h2 << 24);

    mem_storeU64(mac, h0);
    mem_storeU64(mac + 8, h1);
}

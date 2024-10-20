#define chacha20_KEY_SIZE 32

union chacha20 {
    uint8_t u8[64];
    uint32_t u32[16];
    uint64_t u64[8];
    struct {
        uint64_t constant[2];
        uint8_t key[32];
        uint32_t blockCounter;
        uint32_t nonceSender;
        uint64_t nonce;
    } ietf;
    struct {
        uint64_t constant[2];
        uint8_t key[32];
        uint64_t blockCounter;
        uint64_t nonce;
    } orig;
};

#define _chacha20_QUARTER_ROUND(A, B, C, D) \
    A += B; D ^= A; D = hc_ROTL32(D, 16); \
    C += D; B ^= C; B = hc_ROTL32(B, 12); \
    A += B; D ^= A; D = hc_ROTL32(D, 8); \
    C += D; B ^= C; B = hc_ROTL32(B, 7);

static void chacha20_block(const union chacha20 *self, union chacha20 *outStream) {
    union chacha20 local;
    hc_MEMCPY(&local.u64[0], &self->u64[0], 64);

    for (int32_t i = 0; i < 10; ++i) {
        _chacha20_QUARTER_ROUND(local.u32[0], local.u32[4], local.u32[8], local.u32[12])
        _chacha20_QUARTER_ROUND(local.u32[1], local.u32[5], local.u32[9], local.u32[13])
        _chacha20_QUARTER_ROUND(local.u32[2], local.u32[6], local.u32[10], local.u32[14])
        _chacha20_QUARTER_ROUND(local.u32[3], local.u32[7], local.u32[11], local.u32[15])
        _chacha20_QUARTER_ROUND(local.u32[0], local.u32[5], local.u32[10], local.u32[15])
        _chacha20_QUARTER_ROUND(local.u32[1], local.u32[6], local.u32[11], local.u32[12])
        _chacha20_QUARTER_ROUND(local.u32[2], local.u32[7], local.u32[8], local.u32[13])
        _chacha20_QUARTER_ROUND(local.u32[3], local.u32[4], local.u32[9], local.u32[14])
    }

    outStream->u32[0] = local.u32[0] + self->u32[0];
    outStream->u32[1] = local.u32[1] + self->u32[1];
    outStream->u32[2] = local.u32[2] + self->u32[2];
    outStream->u32[3] = local.u32[3] + self->u32[3];
    outStream->u32[4] = local.u32[4] + self->u32[4];
    outStream->u32[5] = local.u32[5] + self->u32[5];
    outStream->u32[6] = local.u32[6] + self->u32[6];
    outStream->u32[7] = local.u32[7] + self->u32[7];
    outStream->u32[8] = local.u32[8] + self->u32[8];
    outStream->u32[9] = local.u32[9] + self->u32[9];
    outStream->u32[10] = local.u32[10] + self->u32[10];
    outStream->u32[11] = local.u32[11] + self->u32[11];
    outStream->u32[12] = local.u32[12] + self->u32[12];
    outStream->u32[13] = local.u32[13] + self->u32[13];
    outStream->u32[14] = local.u32[14] + self->u32[14];
    outStream->u32[15] = local.u32[15] + self->u32[15];
}

static void chacha20_init(union chacha20 *self) {
    self->u64[0] = 0x3320646e61707865;
    self->u64[1] = 0x6b20657479622d32;
}

hc_UNUSED
static void chacha20_initIetf(union chacha20 *self, const void *key, uint32_t blockCounter, uint32_t nonceSender, uint64_t nonce) {
    chacha20_init(self);
    hc_MEMCPY(&self->ietf.key[0], key, chacha20_KEY_SIZE);
    self->ietf.blockCounter = blockCounter;
    self->ietf.nonceSender = nonceSender;
    self->ietf.nonce = nonce;
}

hc_UNUSED
static void chacha20_initOrig(union chacha20 *self, const void *key, uint64_t blockCounter, uint64_t nonce) {
    chacha20_init(self);
    hc_MEMCPY(&self->orig.key[0], key, chacha20_KEY_SIZE);
    self->orig.blockCounter = blockCounter;
    self->orig.nonce = nonce;
}

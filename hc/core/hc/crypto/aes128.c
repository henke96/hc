// Based on code by Romain Dolbeau, public domain.
// NOTE: Does not attempt to avoid cache-timing attacks (https://cr.yp.to/antiforgery/cachetiming-20050414.pdf).

#define aes128_KEY_SIZE 16

static void aes128_block(void *out, const void *in, const void *key) {
    // Expand key.
    uint32_t roundKeys[44];
    uint32_t temp0 = mem_loadU32(key);
    roundKeys[0] = temp0;
    uint32_t temp1 = mem_loadU32(key + 4);
    roundKeys[1] = temp1;
    uint32_t temp2 = mem_loadU32(key + 8);
    roundKeys[2] = temp2;
    uint32_t temp3 = mem_loadU32(key + 12);
    roundKeys[3] = temp3;

    uint32_t pow = 1;
    for (int32_t i = 4; i < 44; i += 4) {
        temp0 ^= pow ^ (
            ((uint32_t)aes_sboxTable[temp3 & 0xFF] << 24) ^
            ((uint32_t)aes_sboxTable[temp3 >> 24] << 16) ^
            ((uint32_t)aes_sboxTable[(temp3 >> 16) & 0xFF] << 8) ^
            (uint32_t)aes_sboxTable[(temp3 >> 8) & 0xFF]
        );

        if (i == 32) pow = 0x1B;
        else pow <<= 1;

        roundKeys[i] = temp0;
        temp1 ^= temp0;
        roundKeys[i + 1] = temp1;
        temp2 ^= temp1;
        roundKeys[i + 2] = temp2;
        temp3 ^= temp2;
        roundKeys[i + 3] = temp3;
    }

    // Do rounds.
    aes_rounds(out, in, &roundKeys[0], 40);
}

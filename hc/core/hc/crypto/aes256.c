// Based on code by Romain Dolbeau, public domain.
// NOTE: Does not attempt to avoid cache-timing attacks (https://cr.yp.to/antiforgery/cachetiming-20050414.pdf).

#define aes256_KEY_SIZE 32

static void aes256_block(void *out, const void *in, const void *key) {
    // Expand key.
    uint32_t roundKeys[60];
    uint32_t temp0 = mem_loadU32(key);
    roundKeys[0] = temp0;
    uint32_t temp1 = mem_loadU32(key + 4);
    roundKeys[1] = temp1;
    uint32_t temp2 = mem_loadU32(key + 8);
    roundKeys[2] = temp2;
    uint32_t temp3 = mem_loadU32(key + 12);
    roundKeys[3] = temp3;
    uint32_t temp4 = mem_loadU32(key + 16);
    roundKeys[4] = temp4;
    uint32_t temp5 = mem_loadU32(key + 20);
    roundKeys[5] = temp5;
    uint32_t temp6 = mem_loadU32(key + 24);
    roundKeys[6] = temp6;
    uint32_t temp7 = mem_loadU32(key + 28);
    roundKeys[7] = temp7;

    uint32_t pow = 1;
    for (int32_t i = 8;; i += 8) {
        temp0 ^= pow ^ (
            ((uint32_t)aes_sboxTable[temp7 & 0xFF] << 24) ^
            ((uint32_t)aes_sboxTable[temp7 >> 24] << 16) ^
            ((uint32_t)aes_sboxTable[(temp7 >> 16) & 0xFF] << 8) ^
            (uint32_t)aes_sboxTable[(temp7 >> 8) & 0xFF]
        );
        pow <<= 1;

        roundKeys[i] = temp0;
        temp1 ^= temp0;
        roundKeys[i + 1] = temp1;
        temp2 ^= temp1;
        roundKeys[i + 2] = temp2;
        temp3 ^= temp2;
        roundKeys[i + 3] = temp3;

        if (i == 56) break;

        temp4 ^= (
            ((uint32_t)aes_sboxTable[temp3 >> 24] << 24) ^
            ((uint32_t)aes_sboxTable[(temp3 >> 16) & 0xFF] << 16) ^
            ((uint32_t)aes_sboxTable[(temp3 >> 8) & 0xFF] << 8) ^
            (uint32_t)aes_sboxTable[temp3 & 0xFF]
        );
        roundKeys[i + 4] = temp4;
        temp5 ^= temp4;
        roundKeys[i + 5] = temp5;
        temp6 ^= temp5;
        roundKeys[i + 6] = temp6;
        temp7 ^= temp6;
        roundKeys[i + 7] = temp7;
    }

    // Do rounds.
    aes_rounds(out, in, &roundKeys[0], 56);
}

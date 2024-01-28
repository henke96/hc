union mul128_u128 {
    uint128_t u128;
    uint64_t u64[2];
};

static uint128_t mul128_64x64(uint64_t a, uint64_t b) {
    union mul128_u128 result;
    result.u64[0] = (a & 0xFFFFFFFF) * (b & 0xFFFFFFFF);
    uint64_t temp = result.u64[0] >> 32;
    result.u64[0] &= 0xFFFFFFFF;
    temp += (a >> 32) * (b & 0xFFFFFFFF);
    result.u64[0] += (temp & 0xFFFFFFFF) << 32;
    result.u64[1] = temp >> 32;
    temp = result.u64[0] >> 32;
    result.u64[0] &= 0xFFFFFFFF;
    temp += (b >> 32) * (a & 0xFFFFFFFF);
    result.u64[0] += (temp & 0xFFFFFFFF) << 32;
    result.u64[1] += temp >> 32;
    result.u64[1] += (a >> 32) * (b >> 32);
    return result.u128;
}

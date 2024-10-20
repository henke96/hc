// ALIGN must be power of 2.
#define math_ALIGN_FORWARD(X, ALIGN) (((X) + ((ALIGN) - 1)) & ~(typeof(X))((ALIGN) - 1))
#define math_ALIGN_BACKWARD(X, ALIGN) ((X) & ~(typeof(X))((ALIGN) - 1))
#define math_ALIGN_REMAINDER(X, ALIGN) ((X) & ((ALIGN) - 1))
#define math_PAD_BYTES(X, ALIGN) (-(X) & ((ALIGN) - 1))
// Check if [X_START, X_END) and [Y_START, Y_END) overlap.
#define math_RANGES_OVERLAP(X_START, X_END, Y_START, Y_END) ((X_START) < (Y_END) && (Y_START) < (X_END))

hc_UNUSED static uint64_t math_gcd64(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

hc_UNUSED static uint32_t math_gcd32(uint32_t a, uint32_t b) {
    while (b != 0) {
        uint32_t temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

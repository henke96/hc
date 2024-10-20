static const char base64_table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define base64_ENCODE_SIZE(INPUT_SIZE) (((INPUT_SIZE + 2) / 3) * 4)

// May be done in-place if the input is placed at the end of the buffer.
static void base64_encode(char *out, char *in, ssize_t inSize) {
    char *end = &in[inSize];

    while (end - in >= 3) {
        uint32_t input0 = in[0];
        uint32_t input1 = in[1];
        uint32_t input2 = in[2];
        out[0] = base64_table[input0 >> 2];
        out[1] = base64_table[((input0 & 0x3) << 4) | (input1 >> 4)];
        out[2] = base64_table[((input1 & 0xF) << 2) | (input2 >> 6)];
        out[3] = base64_table[input2 & 0x3F];

        in += 3;
        out += 4;
    }

    ssize_t remaining = (ssize_t)(end - in);
    if (remaining > 0) {
        uint32_t input0 = in[0];
        out[0] = base64_table[input0 >> 2];
        if (remaining == 1) {
            out[1] = base64_table[(input0 & 0x3) << 4];
            out[2] = '=';
        } else { // remaining == 2
            uint32_t input1 = in[1];
            out[1] = base64_table[((input0 & 0x3) << 4) | (input1 >> 4)];
            out[2] = base64_table[((input1 & 0xF) << 2)];
        }
        out[3] = '=';
    }
}

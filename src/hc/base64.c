static char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define base64_ENCODE_SIZE(INPUT_SIZE) (((INPUT_SIZE + 2) / 3) * 4)

// May be done in-place if the input is placed at the end of the buffer.
static int64_t base64_encode(char *output, char *input, int64_t inputSize) {
    char *end = &input[inputSize];
    char *outputPos = &output[0];

    while (end - input >= 3) {
        uint32_t input0 = input[0];
        uint32_t input1 = input[1];
        uint32_t input2 = input[2];
        outputPos[0] = base64_table[input0 >> 2];
        outputPos[1] = base64_table[((input0 & 0x3) << 4) | (input1 >> 4)];
        outputPos[2] = base64_table[((input1 & 0xF) << 2) | (input2 >> 6)];
        outputPos[3] = base64_table[input2 & 0x3F];

        input += 3;
        outputPos += 4;
    }

    int64_t remaining = (int64_t)(end - input);
    if (remaining > 0) {
        uint32_t input0 = input[0];
        outputPos[0] = base64_table[input0 >> 2];
        if (remaining == 1) {
            outputPos[1] = base64_table[(input0 & 0x3) << 4];
            outputPos[2] = '=';
        } else { // remaining == 2
            uint32_t input1 = input[1];
            outputPos[1] = base64_table[((input0 & 0x3) << 4) | (input1 >> 4)];
            outputPos[2] = base64_table[((input1 & 0xF) << 2)];
        }
        outputPos[3] = '=';
        outputPos += 4;
    }
    return (int64_t)(outputPos - output);
}

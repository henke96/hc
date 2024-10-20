static void mat_init(float *mat) {
    mat[0] = 1.0f;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = 0;

    mat[4] = 0;
    mat[5] = 1.0f;
    mat[6] = 0;
    mat[7] = 0;

    mat[8] = 0;
    mat[9] = 0;
    mat[10] = 1.0f;
    mat[11] = 0;

    mat[12] = 0;
    mat[13] = 0;
    mat[14] = 0;
    mat[15] = 1.0f;
}

hc_UNUSED
static void mat_scale(float *mat, float x, float y, float z) {
    mat[0] *= x;
    mat[5] *= y;
    mat[10] *= z;
}

hc_UNUSED
static void mat_translate(float *mat, float x, float y, float z) {
    mat[12] += x;
    mat[13] += y;
    mat[14] += z;
}

hc_UNUSED
static void mat_rotateX(float *mat, uint32_t pitchAngle) {
    float sin = trig_sin(pitchAngle);
    float cos = trig_cos(pitchAngle);
    float temp1 = mat[1];
    float temp2 = mat[2];
    mat[1] = cos * temp1 - sin * temp2;
    mat[2] = cos * temp2 + sin * temp1;

    temp1 = mat[5];
    temp2 = mat[6];
    mat[5] = cos * temp1 - sin * temp2;
    mat[6] = cos * temp2 + sin * temp1;

    temp1 = mat[9];
    temp2 = mat[10];
    mat[9] = cos * temp1 - sin * temp2;
    mat[10] = cos * temp2 + sin * temp1;

    temp1 = mat[13];
    temp2 = mat[14];
    mat[13] = cos * temp1 - sin * temp2;
    mat[14] = cos * temp2 + sin * temp1;
}

hc_UNUSED
static void mat_rotateY(float *mat, uint32_t yawAngle) {
    float sin = trig_sin(yawAngle);
    float cos = trig_cos(yawAngle);
    float temp1 = mat[0];
    float temp2 = mat[2];
    mat[0] = cos * temp1 + sin * temp2;
    mat[2] = cos * temp2 - sin * temp1;

    temp1 = mat[4];
    temp2 = mat[6];
    mat[4] = cos * temp1 + sin * temp2;
    mat[6] = cos * temp2 - sin * temp1;

    temp1 = mat[8];
    temp2 = mat[10];
    mat[8] = cos * temp1 + sin * temp2;
    mat[10] = cos * temp2 - sin * temp1;

    temp1 = mat[12];
    temp2 = mat[14];
    mat[12] = cos * temp1 + sin * temp2;
    mat[14] = cos * temp2 - sin * temp1;
}

hc_UNUSED
static void mat_rotateZ(float *mat, uint32_t rollAngle) {
    float sin = trig_sin(rollAngle);
    float cos = trig_cos(rollAngle);
    float temp1 = mat[0];
    float temp2 = mat[1];
    mat[0] = cos * temp1 - sin * temp2;
    mat[1] = cos * temp2 + sin * temp1;

    temp1 = mat[4];
    temp2 = mat[5];
    mat[4] = cos * temp1 - sin * temp2;
    mat[5] = cos * temp2 + sin * temp1;

    temp1 = mat[8];
    temp2 = mat[9];
    mat[8] = cos * temp1 - sin * temp2;
    mat[9] = cos * temp2 + sin * temp1;

    temp1 = mat[12];
    temp2 = mat[13];
    mat[12] = cos * temp1 - sin * temp2;
    mat[13] = cos * temp2 + sin * temp1;
}

// Left handed perspective matrix: +X is right, +Y is up, +Z is forward.
static void mat_initPerspective(float *mat, double width, double height, double near, double far) {
    const double fov = 1.0; // 90 degrees.
    mat[0] = (float)fov;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = 0;

    mat[4] = 0;
    mat[5] = (float)(fov * (width / height));
    mat[6] = 0;
    mat[7] = 0;

    mat[8] = 0;
    mat[9] = 0;
    mat[10] = (float)((far + near) / (far - near));
    mat[11] = 1.0f;

    mat[12] = 0;
    mat[13] = 0;
    mat[14] = (float)((-2.0 * near * far) / (far - near));
    mat[15] = 0;
}


static void mat_initOrthographic(float *mat, double width, double height, double depth) {
    mat[0] = (float)(2.0 / width);
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = 0;

    mat[4] = 0;
    mat[5] = (float)(2.0 / height);
    mat[6] = 0;
    mat[7] = 0;

    mat[8] = 0;
    mat[9] = 0;
    mat[10] = (float)(2.0 / depth);
    mat[11] = 0;

    mat[12] = -1.0f;
    mat[13] = -1.0f;
    mat[14] = -1.0f;
    mat[15] = 1.0f;
}

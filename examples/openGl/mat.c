
static void mat_init(float *mat, float scale, float x, float y, float z) {
    mat[0] = scale;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = 0;

    mat[4] = 0;
    mat[5] = scale;
    mat[6] = 0;
    mat[7] = 0;

    mat[8] = 0;
    mat[9] = 0;
    mat[10] = scale;
    mat[11] = 0;

    mat[12] = x;
    mat[13] = y;
    mat[14] = z;
    mat[15] = 1;
}

// Left handed projection matrix: +X is right, +Y is up, +Z is forward.
static void mat_init_projection(float *mat, double near, double far, double width, double height) {
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
    mat[14] = (float)((-2 * near * far) / (far - near));
    mat[15] = 0;
}

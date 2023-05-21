#define shaders_POSITION_LOC 0
#define shaders_MODELVIEW_MATRIX_LOC 1

static const char *shaders_mainVertex =
    "#version 300 es\n"
    "layout (location = " hc_XSTR(shaders_POSITION_LOC) ") in vec3 position;\n"
    "layout (location = " hc_XSTR(shaders_MODELVIEW_MATRIX_LOC) ") in mat4 modelViewMatrix;\n"
    "uniform mat4 projectionMatrix;\n"
    "void main() {\n"
    "    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);\n"
    "}\n";

static const char *shaders_mainFragment =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "    outColor = vec4(1.0, 0.6, 0.2, 1.0);\n"
    "}\n";

static uint32_t shaders_mainProgram;
static int32_t shaders_mainProjectionMatrixLoc;

static int32_t shaders_init(void) {
    // Compile main shader program.
    shaders_mainProgram = gl_createProgram();
    if (shaders_mainProgram == 0) return -1;

    uint32_t vertexShader = gl_createShader(gl_VERTEX_SHADER);
    gl_shaderSource(vertexShader, 1, &shaders_mainVertex, NULL);
    gl_compileShader(vertexShader);

    uint32_t fragmentShader = gl_createShader(gl_FRAGMENT_SHADER);
    gl_shaderSource(fragmentShader, 1, &shaders_mainFragment, NULL);
    gl_compileShader(fragmentShader);

    gl_attachShader(shaders_mainProgram, vertexShader);
    gl_attachShader(shaders_mainProgram, fragmentShader);
    gl_linkProgram(shaders_mainProgram);

    gl_deleteShader(vertexShader);
    gl_deleteShader(fragmentShader);

    shaders_mainProjectionMatrixLoc = gl_getUniformLocation(shaders_mainProgram, "projectionMatrix");
    if (gl_getError() != gl_NO_ERROR) goto cleanup_mainProgram;

    return 0;

    cleanup_mainProgram:
    gl_deleteProgram(shaders_mainProgram);
    return -2;
}

static void shaders_deinit(void) {
    gl_deleteProgram(shaders_mainProgram);
}

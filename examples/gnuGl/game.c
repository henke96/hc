#define game_POSITION_INDEX 0
static const char *game_vertexShader =
    "#version 300 es\n"
    "layout (location = " hc_XSTR(game_POSITION_INDEX) ") in vec3 inPosition;\n"
    "void main() {\n"
    "    gl_Position = vec4(inPosition, 1.0);\n"
    "}\n";
static const char *game_fragmentShader =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "    outColor = vec4(1.0, 0.6, 0.2, 1.0);\n"
    "}\n";

enum game_buffers {
    game_TRIANGLE_BUFFER,
    game_NUM_BUFFERS
};

enum game_vertexArrays {
    game_TRIANGLE_VERTEX_ARRAY,
    game_NUM_VERTEX_ARRAYS
};

static struct {
    uint32_t shaderProgram;
    uint32_t buffers[game_NUM_BUFFERS];
    uint32_t vertexArrays[game_NUM_VERTEX_ARRAYS];
} game;

static int32_t game_init(void) {
    // Compile shader program.
    uint32_t vertexShader = gl_createShader(gl_VERTEX_SHADER);
    gl_shaderSource(vertexShader, 1, &game_vertexShader, NULL);
    gl_compileShader(vertexShader);

    uint32_t fragmentShader = gl_createShader(gl_FRAGMENT_SHADER);
    gl_shaderSource(fragmentShader, 1, &game_fragmentShader, NULL);
    gl_compileShader(fragmentShader);

    game.shaderProgram = gl_createProgram();
    if (game.shaderProgram == 0) return -1;
    gl_attachShader(game.shaderProgram, vertexShader);
    gl_attachShader(game.shaderProgram, fragmentShader);
    gl_linkProgram(game.shaderProgram);

    gl_deleteShader(vertexShader);
    gl_deleteShader(fragmentShader);
    int32_t status;
    if (gl_getError() != gl_NO_ERROR) {
        status = -2;
        goto cleanup_shaderProgram;
    }

    // Initialise buffers and vertex arrays.
    gl_genBuffers(game_NUM_BUFFERS, &game.buffers[0]);
    if (gl_getError() != gl_NO_ERROR) {
        status = -3;
        goto cleanup_shaderProgram;
    }
    gl_genVertexArrays(game_NUM_VERTEX_ARRAYS, &game.vertexArrays[0]);
    if (gl_getError() != gl_NO_ERROR) {
        status = -4;
        goto cleanup_buffers;
    }

    // Setup triangle.
    float triangle[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    gl_bindBuffer(gl_ARRAY_BUFFER, game.buffers[game_TRIANGLE_BUFFER]);
    gl_bufferData(gl_ARRAY_BUFFER, sizeof(triangle), &triangle[0], gl_STATIC_DRAW);

    gl_bindVertexArray(game.vertexArrays[game_TRIANGLE_VERTEX_ARRAY]);
    gl_vertexAttribPointer(
        game_POSITION_INDEX, // index
        3,                   // size
        gl_FLOAT,            // type
        gl_FALSE,            // normalized
        3 * sizeof(float),   // stride
        0                    // pointer (offset into bound gl_ARRAY_BUFFER)
    );
    gl_enableVertexAttribArray(game_POSITION_INDEX);

    // For now we only have one program, so use it from here.
    gl_useProgram(game.shaderProgram);

    if (gl_getError() != gl_NO_ERROR) {
        status = -5;
        goto cleanup_vertexArrays;
    }
    return 0;

    cleanup_vertexArrays:
    gl_deleteVertexArrays(game_NUM_VERTEX_ARRAYS, &game.vertexArrays[0]);
    cleanup_buffers:
    gl_deleteBuffers(game_NUM_BUFFERS, &game.buffers[0]);
    cleanup_shaderProgram:
    gl_deleteProgram(game.shaderProgram);
    return status;
}

static int32_t game_draw(void) {
    gl_clearColor(0.0, 0.0, 0.0, 1.0);
    gl_clear(gl_COLOR_BUFFER_BIT);
    gl_drawArrays(gl_TRIANGLES, 0, 3);
    if (gl_getError() != gl_NO_ERROR) return -1;
    return 0;
}

static void game_deinit(void) {
    gl_deleteVertexArrays(game_NUM_VERTEX_ARRAYS, &game.vertexArrays[0]);
    gl_deleteBuffers(game_NUM_BUFFERS, &game.buffers[0]);
    gl_deleteProgram(game.shaderProgram);
}
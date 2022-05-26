#if !defined(game_EXPORT)
    #error "`#define game_EXPORT(NAME)` before including"
#endif

game_EXPORT("game_draw")
int32_t game_draw(void) {
    float matrix[16];
    mat_init(&matrix[0], 1.0f, -2.0f, 1.0f, 3.5f);
    gl_bufferData(gl_ARRAY_BUFFER, sizeof(matrix), &matrix[0], gl_STREAM_DRAW);

    gl_clear(gl_COLOR_BUFFER_BIT);
    gl_drawElementsInstanced(gl_TRIANGLES, 3, gl_UNSIGNED_SHORT, 0, 1);
    if (gl_getError() != gl_NO_ERROR) return -1;
    return 0;
}

game_EXPORT("game_resize")
void game_resize(int32_t width, int32_t height) {
    gl_viewport(0, 0, width, height);

    float projectionMatrix[16];
    mat_init_projection(&projectionMatrix[0], 0.1, 100.0, width, height);
    gl_uniformMatrix4fv(shaders_mainProjectionMatrixLoc, 1, gl_FALSE, &projectionMatrix[0]);
}

game_EXPORT("game_init")
int32_t game_init(int32_t width, int32_t height) {
    if (shaders_init() < 0) return -1;

    int32_t status;
    if (vertexArrays_init() < 0) {
        status = -2;
        goto cleanup_shaders;
    }

    // Set static OpenGL state.
    gl_useProgram(shaders_mainProgram);
    gl_clearColor(0.0, 0.0, 0.0, 1.0);

    if (gl_getError() != gl_NO_ERROR) {
        status = -3;
        goto cleanup_vertexArrays;
    }
    game_resize(width, height);
    return 0;

    cleanup_vertexArrays:
    vertexArrays_deinit();
    cleanup_shaders:
    shaders_deinit();
    return status;
}

game_EXPORT("game_deinit")
void game_deinit(void) {
    vertexArrays_deinit();
    shaders_deinit();
}
#if !defined(game_EXPORT)
    #error "`#define game_EXPORT(NAME)` before including"
#endif
static struct {
    uint64_t prevFpsCountTime;
    int64_t frameCounter;
    uint32_t cameraYaw;
    int32_t cameraPitch;
    uint32_t triangleYaw;
    int32_t __pad;
} game;

game_EXPORT("game_draw")
int32_t game_draw(uint64_t timestamp) {
    game.triangleYaw = (game.triangleYaw + 1) & 4095;

    float matrix[16];
    mat_init(&matrix[0], 1.0f);
    mat_rotateY(&matrix[0], game.triangleYaw);
    mat_translate(&matrix[0], -2.0f, -0.5f, 3.5f);
    mat_rotateY(&matrix[0], game.cameraYaw);
    mat_rotateX(&matrix[0], game.cameraPitch & 4095);
    gl_bufferData(gl_ARRAY_BUFFER, sizeof(matrix), &matrix[0], gl_STREAM_DRAW);

    gl_clear(gl_COLOR_BUFFER_BIT);
    gl_drawElementsInstanced(gl_TRIANGLES, 3, gl_UNSIGNED_SHORT, 0, 1);
    if (gl_getError() != gl_NO_ERROR) return -1;

    ++game.frameCounter;
    uint64_t nextFpsCountTime = game.prevFpsCountTime + 1000000000;
    if (timestamp >= nextFpsCountTime) {
        debug_printNum("FPS: ", game.frameCounter, "\n");
        game.prevFpsCountTime = nextFpsCountTime;
        game.frameCounter = 0;
    }
    return 0;
}

game_EXPORT("game_onResize")
void game_onResize(int32_t width, int32_t height) {
    gl_viewport(0, 0, width, height);

    float projectionMatrix[16];
    mat_init_projection(&projectionMatrix[0], 0.1, 100.0, width, height);
    gl_uniformMatrix4fv(shaders_mainProjectionMatrixLoc, 1, gl_FALSE, &projectionMatrix[0]);
}

game_EXPORT("game_onMouseMove")
void game_onMouseMove(int64_t deltaX, int64_t deltaY, hc_UNUSED uint64_t timestamp) {
    game.cameraYaw = (game.cameraYaw - (uint32_t)(deltaX >> 32)) & 4095;
    game.cameraPitch -= (deltaY >> 32);
    if (game.cameraPitch > 1024) game.cameraPitch = 1024;
    else if (game.cameraPitch < -1024) game.cameraPitch = -1024;
}

game_EXPORT("game_onKeyDown")
void game_onKeyDown(int32_t key, uint64_t timestamp) {
    debug_printNum("Key down: ", key, "\n");
    debug_printNum("At: ", (int64_t)timestamp, "\n");
}

game_EXPORT("game_onKeyUp")
void game_onKeyUp(int32_t key, uint64_t timestamp) {
    debug_printNum("Key up: ", key, "\n");
    debug_printNum("At: ", (int64_t)timestamp, "\n");
}

game_EXPORT("game_init")
int32_t game_init(int32_t width, int32_t height, uint64_t timestamp) {
    game.cameraYaw = 0;
    game.cameraPitch = 0;
    game.triangleYaw = 0;

    game.prevFpsCountTime = timestamp;
    game.frameCounter = 0;

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
    game_onResize(width, height);
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

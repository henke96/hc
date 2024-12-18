#include "shaders.c"
#include "vertexArrays.c"
#include "trig.c"
#include "mat.c"

#define game_FRAME_HEIGHT 40

static struct {
    float perspectiveMatrix[16];
    float orthographicMatrix[16];
    uint64_t prevFpsCountTime;
    uint64_t frameCounter;
    uint32_t cameraYaw;
    int32_t cameraPitch;
    uint32_t triangleYaw;
    int32_t __pad;
} game;

game_EXPORT void game_draw(uint64_t timestamp, bool drawFrame) {
    game.triangleYaw = (game.triangleYaw + 1) & 4095;

    float matrix[16];
    mat_init(&matrix[0]);
    mat_rotateY(&matrix[0], game.triangleYaw);
    mat_translate(&matrix[0], -2.0f, -0.5f, 3.5f);
    mat_rotateY(&matrix[0], game.cameraYaw);
    mat_rotateX(&matrix[0], game.cameraPitch & 4095);
    gl_bufferData(gl_ARRAY_BUFFER, sizeof(matrix), &matrix[0], gl_STREAM_DRAW);

    gl_clear(gl_COLOR_BUFFER_BIT | gl_DEPTH_BUFFER_BIT);

    gl_uniformMatrix4fv(shaders_mainProjectionMatrixLoc, 1, gl_FALSE, &game.perspectiveMatrix[0]);
    gl_enable(gl_DEPTH_TEST);

    gl_drawElementsInstanced(gl_TRIANGLES, vertexArrays_TRIANGLE_COUNT, gl_UNSIGNED_SHORT, (void *)vertexArrays_TRIANGLE_OFFSET, 1);

    gl_uniformMatrix4fv(shaders_mainProjectionMatrixLoc, 1, gl_FALSE, &game.orthographicMatrix[0]);
    gl_disable(gl_DEPTH_TEST);

    if (drawFrame) {
        mat_init(&matrix[0]);
        mat_translate(&matrix[0], 0.0f, (float)(window_height() - game_FRAME_HEIGHT), 0.0f);
        mat_scale(&matrix[0], (float)(2 * window_width()), 2 * game_FRAME_HEIGHT, 1.0f);
        gl_bufferData(gl_ARRAY_BUFFER, sizeof(matrix), &matrix[0], gl_STREAM_DRAW);
        gl_drawElementsInstanced(gl_TRIANGLES, vertexArrays_FRAME_COUNT, gl_UNSIGNED_SHORT, (void *)vertexArrays_FRAME_OFFSET, 1);

        mat_init(&matrix[0]);
        mat_translate(&matrix[0], (float)(window_width() - game_FRAME_HEIGHT), (float)(window_height() - game_FRAME_HEIGHT), 0.0f);
        mat_scale(&matrix[0], 2 * game_FRAME_HEIGHT, 2 * game_FRAME_HEIGHT, 1.0f);
        gl_bufferData(gl_ARRAY_BUFFER, sizeof(matrix), &matrix[0], gl_STREAM_DRAW);
        gl_drawElementsInstanced(gl_TRIANGLES, vertexArrays_CLOSE_BUTTON_COUNT, gl_UNSIGNED_SHORT, (void *)vertexArrays_CLOSE_BUTTON_OFFSET, 1);
    }

    debug_ASSERT(gl_getError() == gl_NO_ERROR);

    ++game.frameCounter;
    uint64_t nextFpsCountTime = game.prevFpsCountTime + 1000000000;
    if (timestamp >= nextFpsCountTime) {
        char print[hc_STR_LEN("FPS: ") + util_UINT64_MAX_CHARS + hc_STR_LEN("\n")];
        char *pos = hc_ARRAY_END(print);
        hc_PREPEND_STR(pos, "\n");
        pos = util_uintToStr(pos, game.frameCounter);
        hc_PREPEND_STR(pos, "FPS: ");
        debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);

        game.prevFpsCountTime = nextFpsCountTime;
        game.frameCounter = 0;
    }
}

game_EXPORT void game_onResize(void) {
    int32_t width = window_width();
    int32_t height = window_height();
    gl_viewport(0, 0, width, height);

    mat_initPerspective(&game.perspectiveMatrix[0], width, height, 0.1, 100.0);
    mat_initOrthographic(&game.orthographicMatrix[0], width, height, 100.0);
}

game_EXPORT void game_onMouseMove(int64_t deltaX, int64_t deltaY, hc_UNUSED uint64_t timestamp) {
    game.cameraYaw = (game.cameraYaw - (uint32_t)(deltaX >> 32)) & 4095;
    game.cameraPitch -= (deltaY >> 32);
    if (game.cameraPitch > 1024) game.cameraPitch = 1024;
    else if (game.cameraPitch < -1024) game.cameraPitch = -1024;
}

game_EXPORT void game_onKeyDown(int32_t key, uint64_t timestamp) {
    char print[
        hc_STR_LEN("Key down: ") +
        util_INT32_MAX_CHARS +
        hc_STR_LEN(", at: ") +
        util_UINT64_MAX_CHARS +
        hc_STR_LEN("\n")
    ];
    char *pos = hc_ARRAY_END(print);
    hc_PREPEND_STR(pos, "\n");
    pos = util_uintToStr(pos, timestamp);
    hc_PREPEND_STR(pos, ", at: ");
    pos = util_intToStr(pos, key);
    hc_PREPEND_STR(pos, "Key down: ");
    debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
}

game_EXPORT void game_onKeyUp(int32_t key, uint64_t timestamp) {
    char print[
        hc_STR_LEN("Key up: ") +
        util_INT32_MAX_CHARS +
        hc_STR_LEN(", at: ") +
        util_UINT64_MAX_CHARS +
        hc_STR_LEN("\n")
    ];
    char *pos = hc_ARRAY_END(print);
    hc_PREPEND_STR(pos, "\n");
    pos = util_uintToStr(pos, timestamp);
    hc_PREPEND_STR(pos, ", at: ");
    pos = util_intToStr(pos, key);
    hc_PREPEND_STR(pos, "Key up: ");
    debug_CHECK(util_writeAll(util_STDERR, pos, hc_ARRAY_END(print) - pos), RES == 0);
}

game_EXPORT int32_t game_init(uint64_t timestamp) {
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
    game_onResize();
    return 0;

    cleanup_vertexArrays:
    vertexArrays_deinit();
    cleanup_shaders:
    shaders_deinit();
    return status;
}

game_EXPORT void game_deinit(void) {
    vertexArrays_deinit();
    shaders_deinit();
}

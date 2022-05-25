#if !defined(game_EXPORT)
    #error "`#define game_EXPORT(NAME)` before including"
#endif

static const char *game_vertexShader =
    "#version 300 es\n"
    "layout (location = " hc_XSTR(vertexArrays_POSITION_INDEX) ") in vec3 position;\n"
    "layout (location = " hc_XSTR(vertexArrays_MODELVIEW_MATRIX_INDEX) ") in mat4 modelViewMatrix;\n"
    "void main() {\n"
    "    gl_Position = modelViewMatrix * vec4(position, 1.0);\n"
    "}\n";
static const char *game_fragmentShader =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "    outColor = vec4(1.0, 0.6, 0.2, 1.0);\n"
    "}\n";

game_EXPORT("game_init")
int32_t game_init(void) {
    if (shaders_init() < 0) return -1;

    int32_t status;
    if (vertexArrays_init() < 0) {
        status = -2;
        goto cleanup_shaders;
    }

    // For now we only have one program, so use it from here.
    gl_useProgram(shaders_mainProgram);
    gl_clearColor(0.0, 0.0, 0.0, 1.0);

    if (gl_getError() != gl_NO_ERROR) {
        status = -3;
        goto cleanup_vertexArrays;
    }
    return 0;

    cleanup_vertexArrays:
    vertexArrays_deinit();
    cleanup_shaders:
    shaders_deinit();
    return status;
}

game_EXPORT("game_draw")
int32_t game_draw(void) {
    gl_clear(gl_COLOR_BUFFER_BIT);
    gl_drawElementsInstanced(gl_TRIANGLES, 3, gl_UNSIGNED_SHORT, 0, 1);
    if (gl_getError() != gl_NO_ERROR) return -1;
    return 0;
}

game_EXPORT("game_resize")
void game_resize(int32_t width, int32_t height) {
    gl_viewport(0, 0, width, height);
}

game_EXPORT("game_deinit")
void game_deinit(void) {
    vertexArrays_deinit();
    shaders_deinit();
}
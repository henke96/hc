#include "../../../src/hc.h"
#include "../../../src/gl.h"

#define game_EXPORT(NAME) hc_WASM_EXPORT(NAME)
#include "gl.h"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../game.c"

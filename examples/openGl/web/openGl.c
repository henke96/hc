#include "hc/hc.h"
#include "hc/gl.h"
#include "hc/debug.h"

#define game_EXPORT(NAME) hc_WASM_EXPORT(NAME)
#include "debug.c"
#include "gl.h"
#include "../shaders.c"
#include "../vertexArrays.c"
#include "../trig.c"
#include "../mat.c"
#include "../game.c"

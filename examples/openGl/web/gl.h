hc_WASM_IMPORT("env", "gl_getError")
uint32_t gl_getError(void);

hc_WASM_IMPORT("env", "gl_clear")
void gl_clear(uint32_t mask);

hc_WASM_IMPORT("env", "gl_clearColor")
void gl_clearColor(float red, float green, float blue, float alpha);

hc_WASM_IMPORT("env", "gl_viewport")
void gl_viewport(int32_t x, int32_t y, int32_t width, int32_t height);

hc_WASM_IMPORT("env", "gl_genBuffers")
void gl_genBuffers(int32_t n, uint32_t *buffers);

hc_WASM_IMPORT("env", "gl_bindBuffer")
void gl_bindBuffer(uint32_t target, uint32_t buffer);

hc_WASM_IMPORT("env", "gl_bufferData")
void gl_bufferData(uint32_t target, int32_t size, const void *data, uint32_t usage);

hc_WASM_IMPORT("env", "gl_createShader")
uint32_t gl_createShader(uint32_t type);

hc_WASM_IMPORT("env", "gl_shaderSource")
void gl_shaderSource(uint32_t shader, int32_t count, const char *const *string, const int32_t *length);

hc_WASM_IMPORT("env", "gl_compileShader")
void gl_compileShader(uint32_t shader);

hc_WASM_IMPORT("env", "gl_createProgram")
uint32_t gl_createProgram(void);

hc_WASM_IMPORT("env", "gl_attachShader")
void gl_attachShader(uint32_t program, uint32_t shader);

hc_WASM_IMPORT("env", "gl_linkProgram")
void gl_linkProgram(uint32_t program);

hc_WASM_IMPORT("env", "gl_useProgram")
void gl_useProgram(uint32_t program);

hc_WASM_IMPORT("env", "gl_deleteProgram")
void gl_deleteProgram(uint32_t program);

hc_WASM_IMPORT("env", "gl_deleteShader")
void gl_deleteShader(uint32_t shader);

hc_WASM_IMPORT("env", "gl_vertexAttribPointer")
void gl_vertexAttribPointer(uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, const void *pointer);

hc_WASM_IMPORT("env", "gl_enableVertexAttribArray")
void gl_enableVertexAttribArray(uint32_t index);

hc_WASM_IMPORT("env", "gl_genVertexArrays")
void gl_genVertexArrays(int32_t n, uint32_t *arrays);

hc_WASM_IMPORT("env", "gl_bindVertexArray")
void gl_bindVertexArray(uint32_t array);

hc_WASM_IMPORT("env", "gl_deleteVertexArrays")
void gl_deleteVertexArrays(int32_t n, uint32_t *arrays);

hc_WASM_IMPORT("env", "gl_deleteBuffers")
void gl_deleteBuffers(int32_t n, uint32_t *buffers);

hc_WASM_IMPORT("env", "gl_drawElementsInstanced")
void gl_drawElementsInstanced(uint32_t mode, int32_t count, uint32_t type, const void *indices, int32_t primcount);

hc_WASM_IMPORT("env", "gl_vertexAttribDivisor")
void gl_vertexAttribDivisor(uint32_t index, uint32_t divisor);

hc_WASM_IMPORT("env", "gl_uniformMatrix4fv")
void gl_uniformMatrix4fv(int32_t location, int32_t count, uint8_t transpose, const float *value);

hc_WASM_IMPORT("env", "gl_getUniformLocation")
int32_t gl_getUniformLocation(uint32_t program, const char *name);

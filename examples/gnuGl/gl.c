static uint32_t (*gl_getError)(void);
static void (*gl_clear)(uint32_t mask);
static void (*gl_clearColor)(float red, float green, float blue, float alpha);
static void (*gl_viewport)(int32_t x, int32_t y, int32_t width, int32_t height);
static void (*gl_genBuffers)(int32_t n, uint32_t *buffers);
static void (*gl_bindBuffer)(uint32_t target, uint32_t buffer);
static void (*gl_bufferData)(uint32_t target, int64_t size, const void *data, uint32_t usage);
static uint32_t (*gl_createShader)(uint32_t type);
static void (*gl_shaderSource)(uint32_t shader, int32_t count, const char *const *string, const int32_t *length);
static void (*gl_compileShader)(uint32_t shader);
static uint32_t (*gl_createProgram)(void);
static void (*gl_attachShader)(uint32_t program, uint32_t shader);
static void (*gl_linkProgram)(uint32_t program);
static void (*gl_useProgram)(uint32_t program);
static void (*gl_deleteProgram)(uint32_t program);
static void (*gl_deleteShader)(uint32_t shader);
static void (*gl_vertexAttribPointer)(uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, const void *pointer);
static void (*gl_enableVertexAttribArray)(uint32_t index);
static void (*gl_genVertexArrays)(int32_t n, uint32_t *arrays);
static void (*gl_bindVertexArray)(uint32_t array);
static void (*gl_deleteVertexArrays)(int32_t n, uint32_t *arrays);
static void (*gl_deleteBuffers)(int32_t n, uint32_t *buffers);
static void (*gl_drawArrays)(uint32_t mode, int32_t first, int32_t count);

static int32_t gl_init(struct egl *egl) {
    if ((gl_getError = egl_getProcAddress(egl, "glGetError")) == NULL) return -1;
    if ((gl_clear = egl_getProcAddress(egl, "glClear")) == NULL) return -1;
    if ((gl_clearColor = egl_getProcAddress(egl, "glClearColor")) == NULL) return -1;
    if ((gl_viewport = egl_getProcAddress(egl, "glViewport")) == NULL) return -1;
    if ((gl_genBuffers = egl_getProcAddress(egl, "glGenBuffers")) == NULL) return -1;
    if ((gl_bindBuffer = egl_getProcAddress(egl, "glBindBuffer")) == NULL) return -1;
    if ((gl_bufferData = egl_getProcAddress(egl, "glBufferData")) == NULL) return -1;
    if ((gl_createShader = egl_getProcAddress(egl, "glCreateShader")) == NULL) return -1;
    if ((gl_shaderSource = egl_getProcAddress(egl, "glShaderSource")) == NULL) return -1;
    if ((gl_compileShader = egl_getProcAddress(egl, "glCompileShader")) == NULL) return -1;
    if ((gl_createProgram = egl_getProcAddress(egl, "glCreateProgram")) == NULL) return -1;
    if ((gl_attachShader = egl_getProcAddress(egl, "glAttachShader")) == NULL) return -1;
    if ((gl_linkProgram = egl_getProcAddress(egl, "glLinkProgram")) == NULL) return -1;
    if ((gl_useProgram = egl_getProcAddress(egl, "glUseProgram")) == NULL) return -1;
    if ((gl_deleteProgram = egl_getProcAddress(egl, "glDeleteProgram")) == NULL) return -1;
    if ((gl_deleteShader = egl_getProcAddress(egl, "glDeleteShader")) == NULL) return -1;
    if ((gl_vertexAttribPointer = egl_getProcAddress(egl, "glVertexAttribPointer")) == NULL) return -1;
    if ((gl_enableVertexAttribArray = egl_getProcAddress(egl, "glEnableVertexAttribArray")) == NULL) return -1;
    if ((gl_genVertexArrays = egl_getProcAddress(egl, "glGenVertexArrays")) == NULL) return -1;
    if ((gl_bindVertexArray = egl_getProcAddress(egl, "glBindVertexArray")) == NULL) return -1;
    if ((gl_deleteVertexArrays = egl_getProcAddress(egl, "glDeleteVertexArrays")) == NULL) return -1;
    if ((gl_deleteBuffers = egl_getProcAddress(egl, "glDeleteBuffers")) == NULL) return -1;
    if ((gl_drawArrays = egl_getProcAddress(egl, "glDrawArrays")) == NULL) return -1;
    return 0;
}
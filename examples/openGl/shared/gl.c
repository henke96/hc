#if !defined(gl_GET_PROC_ADDR)
    #error "Define `gl_GET_PROC_ADDR(loader, FUNC)`"
#endif

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
static void (*gl_drawElementsInstanced)(uint32_t mode, int32_t count, uint32_t type, const void *indices, int32_t primcount);
static void (*gl_vertexAttribDivisor)(uint32_t index, uint32_t divisor);
static void (*gl_uniformMatrix4fv)(int32_t location, int32_t count, uint8_t transpose, const float *value);
static int32_t (*gl_getUniformLocation)(uint32_t program, const char *name);

static int32_t gl_init(void *loader) {
    if ((gl_getError = gl_GET_PROC_ADDR(loader, "glGetError")) == NULL) return -1;
    if ((gl_clear = gl_GET_PROC_ADDR(loader, "glClear")) == NULL) return -1;
    if ((gl_clearColor = gl_GET_PROC_ADDR(loader, "glClearColor")) == NULL) return -1;
    if ((gl_viewport = gl_GET_PROC_ADDR(loader, "glViewport")) == NULL) return -1;
    if ((gl_genBuffers = gl_GET_PROC_ADDR(loader, "glGenBuffers")) == NULL) return -1;
    if ((gl_bindBuffer = gl_GET_PROC_ADDR(loader, "glBindBuffer")) == NULL) return -1;
    if ((gl_bufferData = gl_GET_PROC_ADDR(loader, "glBufferData")) == NULL) return -1;
    if ((gl_createShader = gl_GET_PROC_ADDR(loader, "glCreateShader")) == NULL) return -1;
    if ((gl_shaderSource = gl_GET_PROC_ADDR(loader, "glShaderSource")) == NULL) return -1;
    if ((gl_compileShader = gl_GET_PROC_ADDR(loader, "glCompileShader")) == NULL) return -1;
    if ((gl_createProgram = gl_GET_PROC_ADDR(loader, "glCreateProgram")) == NULL) return -1;
    if ((gl_attachShader = gl_GET_PROC_ADDR(loader, "glAttachShader")) == NULL) return -1;
    if ((gl_linkProgram = gl_GET_PROC_ADDR(loader, "glLinkProgram")) == NULL) return -1;
    if ((gl_useProgram = gl_GET_PROC_ADDR(loader, "glUseProgram")) == NULL) return -1;
    if ((gl_deleteProgram = gl_GET_PROC_ADDR(loader, "glDeleteProgram")) == NULL) return -1;
    if ((gl_deleteShader = gl_GET_PROC_ADDR(loader, "glDeleteShader")) == NULL) return -1;
    if ((gl_vertexAttribPointer = gl_GET_PROC_ADDR(loader, "glVertexAttribPointer")) == NULL) return -1;
    if ((gl_enableVertexAttribArray = gl_GET_PROC_ADDR(loader, "glEnableVertexAttribArray")) == NULL) return -1;
    if ((gl_genVertexArrays = gl_GET_PROC_ADDR(loader, "glGenVertexArrays")) == NULL) return -1;
    if ((gl_bindVertexArray = gl_GET_PROC_ADDR(loader, "glBindVertexArray")) == NULL) return -1;
    if ((gl_deleteVertexArrays = gl_GET_PROC_ADDR(loader, "glDeleteVertexArrays")) == NULL) return -1;
    if ((gl_deleteBuffers = gl_GET_PROC_ADDR(loader, "glDeleteBuffers")) == NULL) return -1;
    if ((gl_drawElementsInstanced = gl_GET_PROC_ADDR(loader, "glDrawElementsInstanced")) == NULL) return -1;
    if ((gl_vertexAttribDivisor = gl_GET_PROC_ADDR(loader, "glVertexAttribDivisor")) == NULL) return -1;
    if ((gl_uniformMatrix4fv = gl_GET_PROC_ADDR(loader, "glUniformMatrix4fv")) == NULL) return -1;
    if ((gl_getUniformLocation = gl_GET_PROC_ADDR(loader, "glGetUniformLocation")) == NULL) return -1;
    return 0;
}

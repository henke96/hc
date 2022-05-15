static void (*gl_clear)(uint32_t mask);
static void (*gl_clearColor)(float red, float green, float blue, float alpha);
static void (*gl_viewport)(int32_t x, int32_t y, int32_t width, int32_t height);

static int32_t gl_init(struct egl *egl) {
    gl_clear = egl_getProcAddress(egl, "glClear");
    if (gl_clear == NULL) return -1;
    gl_clearColor = egl_getProcAddress(egl, "glClearColor");
    if (gl_clearColor == NULL) return -2;
    gl_viewport = egl_getProcAddress(egl, "glViewport");
    if (gl_viewport == NULL) return -3;
    return 0;
}
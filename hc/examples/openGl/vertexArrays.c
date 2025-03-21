enum vertexArrays {
    vertexArrays_STATIC,
    vertexArrays_NUM
};

enum vertexArrays_buffers {
    vertexArrays_VERTEX_BUFFER,
    vertexArrays_INDEX_BUFFER,
    vertexArrays_MODELVIEW_MATRIX_BUFFER,
    vertexArrays_NUM_BUFFERS
};

static uint32_t vertexArrays[vertexArrays_NUM];
static uint32_t vertexArrays_buffers[vertexArrays_NUM][vertexArrays_NUM_BUFFERS];

static int32_t vertexArrays_init(void) {
    gl_genVertexArrays(vertexArrays_NUM, &vertexArrays[0]);
    if (gl_getError() != gl_NO_ERROR) return -1;

    int32_t status;
    gl_genBuffers((int32_t)vertexArrays_NUM * (int32_t)vertexArrays_NUM_BUFFERS, &vertexArrays_buffers[0][0]);
    if (gl_getError() != gl_NO_ERROR) {
        status = -2;
        goto cleanup_vertexArrays;
    }

    // vertexArrays_STATIC
    {
        #define VERTEX_ARRAY vertexArrays_STATIC
        gl_bindVertexArray(vertexArrays[VERTEX_ARRAY]);

        // Vertex buffer.
        gl_bindBuffer(gl_ARRAY_BUFFER, vertexArrays_buffers[VERTEX_ARRAY][vertexArrays_VERTEX_BUFFER]);
        gl_enableVertexAttribArray(shaders_POSITION_LOC);
        gl_vertexAttribPointer(
            shaders_POSITION_LOC,
            3,
            gl_SHORT,
            gl_TRUE,
            12,
            0
        );
        gl_enableVertexAttribArray(shaders_COLOUR_LOC);
        gl_vertexAttribPointer(
            shaders_COLOUR_LOC,
            3,
            gl_SHORT,
            gl_TRUE,
            12,
            (void *)6
        );

        #define ONE INT16_MAX
        #define HALF (INT16_MAX / 2)
        #define FRAME_COLOUR INT16_MAX / 6, INT16_MAX / 6, INT16_MAX / 6
        #define CLOSE_BUTTON_COLOUR HALF, 0, 0
        int16_t vertices[] = {
            0, 0, 0,   FRAME_COLOUR,
            ONE, 0, 0, FRAME_COLOUR,
            0, ONE, 0, FRAME_COLOUR,

            0, 0, 0,     CLOSE_BUTTON_COLOUR,
            ONE, 0, 0,   CLOSE_BUTTON_COLOUR,
            0, ONE, 0,   CLOSE_BUTTON_COLOUR,

            -HALF, -HALF, 0, HALF, HALF, HALF,
            HALF, -HALF, 0,  HALF, HALF, HALF,
            0, HALF, 0,      HALF, HALF, HALF
        };
        #undef ONE
        #undef HALF
        #undef FRAME_COLOUR
        #undef CLOSE_BUTTON_COLOUR
        gl_bufferData(gl_ARRAY_BUFFER, sizeof(vertices), &vertices[0], gl_STATIC_DRAW);

        // Index buffer.
        gl_bindBuffer(gl_ELEMENT_ARRAY_BUFFER, vertexArrays_buffers[VERTEX_ARRAY][vertexArrays_INDEX_BUFFER]);
        uint16_t indices[] = {
            #define vertexArrays_FRAME_OFFSET 0
            #define vertexArrays_FRAME_COUNT 3
            0, 1, 2,
            #define vertexArrays_CLOSE_BUTTON_OFFSET (vertexArrays_FRAME_OFFSET + 2 * vertexArrays_FRAME_COUNT)
            #define vertexArrays_CLOSE_BUTTON_COUNT 3
            3, 4, 5,
            #define vertexArrays_TRIANGLE_OFFSET (vertexArrays_CLOSE_BUTTON_OFFSET + 2 * vertexArrays_CLOSE_BUTTON_COUNT)
            #define vertexArrays_TRIANGLE_COUNT 3
            6, 7, 8
        };
        gl_bufferData(gl_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], gl_STATIC_DRAW);

        // Model-View matrix buffer.
        gl_bindBuffer(gl_ARRAY_BUFFER, vertexArrays_buffers[VERTEX_ARRAY][vertexArrays_MODELVIEW_MATRIX_BUFFER]);
        for (uint32_t i = 0; i < 4; ++i) {
            uint32_t loc = shaders_MODELVIEW_MATRIX_LOC + i;
            gl_enableVertexAttribArray(loc);
            gl_vertexAttribPointer(
                loc,
                4,
                gl_FLOAT,
                gl_FALSE,
                4 * 4 * sizeof(float),
                (void *)(size_t)(i * 4 * sizeof(float))
            );
            gl_vertexAttribDivisor(loc, 1);
        }
        #undef VERTEX_ARRAY
    }

    if (gl_getError() != gl_NO_ERROR) {
        status = -3;
        goto cleanup_buffers;
    }
    return 0;

    cleanup_buffers:
    gl_deleteBuffers((int32_t)vertexArrays_NUM * (int32_t)vertexArrays_NUM_BUFFERS, &vertexArrays_buffers[0][0]);
    cleanup_vertexArrays:
    gl_deleteVertexArrays(vertexArrays_NUM, &vertexArrays[0]);
    return status;
}

static void vertexArrays_deinit(void) {
    gl_deleteBuffers((int32_t)vertexArrays_NUM * (int32_t)vertexArrays_NUM_BUFFERS, &vertexArrays_buffers[0][0]);
    gl_deleteVertexArrays(vertexArrays_NUM, &vertexArrays[0]);
}

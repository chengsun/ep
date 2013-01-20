#include "demo.h"

const float vertexPositions[] = {
    0.75f, 0.75f, 0.0f, 1.0f,
    0.75f, -0.75f, 0.0f, 1.0f,
    -0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;
GLuint triangleProgram;

void demo_init(unsigned w, unsigned h)
{
    // create shader program

    GLuint shaders[] = {
        CreateShader(GL_VERTEX_SHADER, "data/triangle.vs"),
        CreateShader(GL_FRAGMENT_SHADER, "data/triangle.fs")
    };
    triangleProgram = CreateProgram(shaders, ARRAYLEN(shaders));
    for (unsigned i = 0; i < ARRAYLEN(shaders); ++i)
        glDeleteShader(shaders[i]);

    // initialise vertex buffer

    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(triangleProgram);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

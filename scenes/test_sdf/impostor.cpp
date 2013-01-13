#include "demo.h"

const float vertexPositions[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, +1.0f, 0.0f, 1.0f,
    +1.0f, +1.0f, 0.0f, 1.0f,
    +1.0f, -1.0f, 0.0f, 1.0f
};

GLuint positionBufferObject;
GLuint vao;
GLuint triangleProgram;
GLuint timeUniform, frame = 0;

void demo_init(unsigned w, unsigned h)
{
    // create shader program

    GLuint shaders[] = {
        CreateShader(GL_VERTEX_SHADER, "data/impostor.vs"),
        CreateShader(GL_FRAGMENT_SHADER, "data/impostor.fs")
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

    timeUniform = glGetUniformLocation(triangleProgram, "time");
}

void demo_prepareFrame()
{
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(triangleProgram);

    if (timeUniform != -1u) glUniform1f(timeUniform, frame++);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

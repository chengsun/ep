#include "demo.h"

const float vertexPositions[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, +1.0f, 0.0f, 1.0f,
    +1.0f, +1.0f, 0.0f, 1.0f,
    +1.0f, -1.0f, 0.0f, 1.0f
};

GLuint positionBufferObject;
GLuint vao;
Program *triangleProgram;
GLuint timeUniform, frame = 0;

void demo_init(unsigned w, unsigned h)
{
    // create shader program
    triangleProgram = new Program({
        Shader(GL_VERTEX_SHADER, "data/impostor.vs"),
        Shader(GL_FRAGMENT_SHADER, "data/impostor.fs")
    });

    // initialise vertex buffer

    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    timeUniform = glGetUniformLocation(triangleProgram->id, "time");
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(triangleProgram->id);

    if (timeUniform != -1u) glUniform1f(timeUniform, frame++);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

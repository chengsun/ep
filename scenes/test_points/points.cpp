#include "demo.h"

char *vertexPositions;

GLuint positionBufferObject;
GLuint vao, timeUniform, frame = 0;
GLuint triangleProgram;

#define NPOINTS 5000000

void demo_init(unsigned w, unsigned h)
{
    // create shader program

    GLuint shaders[] = {
        CreateShader(GL_VERTEX_SHADER, "data/points.vs"),
        CreateShader(GL_FRAGMENT_SHADER, "data/points.fs")
    };
    triangleProgram = CreateProgram(shaders, ARRAYLEN(shaders));
    for (unsigned i = 0; i < ARRAYLEN(shaders); ++i)
        glDeleteShader(shaders[i]);

    // initialise vertex buffer
    vertexPositions = new char[NPOINTS];
    for (int i = 0; i < NPOINTS; ++i) {
        vertexPositions[i] = i;
    }

    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, NPOINTS, vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    glDisable(GL_PROGRAM_POINT_SIZE);
    glPointSize(2.0f);

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
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

    if (timeUniform != -1u) glUniform1f(timeUniform, frame++);

    glDrawArrays(GL_POINTS, 0, NPOINTS);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

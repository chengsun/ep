#include "demo.h"

char *vertexPositions;

GLuint positionBufferObject;
GLuint vao, timeUniform, frame = 0;
Program *triangleProgram;

#define NPOINTS 5000000

void demo_init(unsigned w, unsigned h)
{
    // create shader program
    triangleProgram = new ProgramRaw({
        Shader::FromFile(GL_VERTEX_SHADER, "data/points.vs"),
        Shader::FromFile(GL_FRAGMENT_SHADER, "data/points.fs")
    });

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
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

    if (timeUniform != -1u) glUniform1f(timeUniform, frame++);

    glDrawArrays(GL_POINTS, 0, NPOINTS);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

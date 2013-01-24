#ifndef DEMOLIB_PROGRAM_H
#define DEMOLIB_PROGRAM_H

#include <GL/gl.h>
#include <vector>

struct Shader
{
    Shader(GLenum _type, const char *_filename);
    ~Shader();

    GLuint id;
    GLenum type;
    const char *filename;
};

struct Program
{
    Program(std::initializer_list<Shader> &&_shaders);

    GLuint id;
    std::vector<Shader> shaders;
};

#endif

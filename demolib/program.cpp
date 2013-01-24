#include "demolib.h"
#include <cstdio>

Shader::Shader(GLenum _type, const char *_filename) :
    type(_type),
    filename(_filename)
{
    const char *strType = "unknown";
    switch (type) {
        case GL_VERTEX_SHADER: strType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strType = "fragment"; break;
    }

    LOG("Opening %s shader program \"%s\"", strType, filename);

    FILE *file = fopen(filename, "rb");
    ASSERTX(file, "Failed to open %s shader program \"%s\"", strType, filename);
    fseek(file, 0, SEEK_END);
    unsigned long fileLength = ftell(file);
    rewind(file);

    GLchar *shaderCode = new GLchar[fileLength];
    int shaderReadLength = fread(shaderCode, 1, fileLength, file);
    fclose(file);

    LOG("Compiling %s shader program \"%s\"", strType, filename);

    id= glCreateShader(type);
    glShaderSource(id, 1, const_cast<const GLchar **>(&shaderCode), &shaderReadLength);

    delete[] shaderCode;

    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);

        LOG("WARNING: Shader compile failure in %s shader \"%s\": %s",
            strType, filename, strInfoLog);
        delete[] strInfoLog;
    }
}

Shader::~Shader()
{
    glDeleteShader(id);
}

Program::Program(std::initializer_list<Shader> &&_shaders) :
    shaders(_shaders)
{
    LOG("Linking shader program");

    id = glCreateProgram();

    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glAttachShader(id, shaders[shaderIdx].id);

    glLinkProgram(id);

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(id, infoLogLength, NULL, strInfoLog);
        LOG("WARNING: Shader linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glDetachShader(id, shaders[shaderIdx].id);
}

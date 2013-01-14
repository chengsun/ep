#include "demolib.h"
#include <cstdio>

GLuint CreateShader(GLenum eShaderType, const char *strShaderFile)
{
    const char *strShaderType = "unknown";
    switch(eShaderType)
    {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
    }

    LOG("Opening %s shader program \"%s\"", strShaderType, strShaderFile);

    FILE *shaderFile = fopen(strShaderFile, "rb");
    if (!shaderFile) {
        LOG("WARNING: Failed to open %s shader program \"%s\"", strShaderType, strShaderFile);
        return 0;
    }
    fseek(shaderFile, 0, SEEK_END);
    unsigned long shaderFileLength = ftell(shaderFile);
    rewind(shaderFile);

    GLchar *shaderCode = new GLchar[shaderFileLength];
    int shaderReadLength = fread(shaderCode, 1, shaderFileLength, shaderFile);
    fclose(shaderFile);

    LOG("Compiling %s shader program \"%s\"", strShaderType, strShaderFile);

    GLuint shader = glCreateShader(eShaderType);
    glShaderSource(shader, 1, const_cast<const GLchar **>(&shaderCode), &shaderReadLength);

    delete[] shaderCode;

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        LOG("WARNING: Shader compile failure in %s shader \"%s\": %s",
            strShaderType, strShaderFile, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}

GLuint CreateProgram(GLuint *shaderList, unsigned nShaders)
{
    LOG("Linking shader program");

    GLuint program = glCreateProgram();

    for (unsigned iLoop = 0; iLoop < nShaders; iLoop++)
        glAttachShader(program, shaderList[iLoop]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        LOG("WARNING: Shader linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for(size_t iLoop = 0; iLoop < nShaders; iLoop++)
        glDetachShader(program, shaderList[iLoop]);

    return program;
}

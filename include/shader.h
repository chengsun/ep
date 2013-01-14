#ifndef DEMOLIB_SHADER_H
#define DEMOLIB_SHADER_H

extern "C"
{
    GLuint CreateShader(GLenum eShaderType, const char *strShaderFile);
    GLuint CreateProgram(GLuint *shaderList, unsigned nShaders);
}

#endif

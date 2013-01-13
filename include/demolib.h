#ifndef DEMOLIB_H
#define DEMOLIB_H

#include <GL/glew.h>

#define ARRAYLEN(x) (sizeof(x)/sizeof((x)[0]))

// util.cpp
extern "C"
{
    void dlib_log(const char *file, unsigned line, const char *format, ...);
}

#ifdef log
#undef log
#endif

#ifdef assert
#undef assert
#endif

#ifdef DEBUG
#   include <cstdlib>
#   define log(...) dlib_log(__FILE__, __LINE__, __VA_ARGS__)
#   define assert(x, ...) \
    do { \
        if (!(x)) { \
            log("\n* ASSERTION FAILED! *\n " __VA_ARGS__); \
            abort(); \
        } \
    } while (0)
#else
#   define log(...)    ((void)0)
#   define assert(x, ...) ((void)0)
#endif


// shader.cpp
extern "C"
{
    GLuint CreateShader(GLenum eShaderType, const char *strShaderFile);
    GLuint CreateProgram(GLuint *shaderList, unsigned nShaders);
}


#endif

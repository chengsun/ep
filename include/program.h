#ifndef DEMOLIB_PROGRAM_H
#define DEMOLIB_PROGRAM_H

#include "mesh.h"
#include <GL/gl.h>
#include <vector>
#include <string>

struct Shader
{
protected:
    Shader(GLenum _type, const char *_name);
public:
    ~Shader();

    bool compile(std::string shaderCode);

    GLuint id;
    GLenum type;
    const char *name, *strType;

    static Shader FromFile(GLenum type, const char *filename);
    static Shader Inline(GLenum type, std::string shaderCode, const char *name = "<inline>");
};

struct Program
{
    static constexpr U32 PrimitiveRestartIndex = 0xFFFFFFFF;

protected:
    Program(std::initializer_list<Shader> &&_shaders);

public:
    virtual void updateMeshBuf(const Mesh &mesh) = 0;
    virtual void draw() const = 0;

    GLuint id;

private:
    std::vector<Shader> shaders;
};

struct ProgramRaw : public Program
{
    ProgramRaw(std::initializer_list<Shader> &&_shaders) :
        Program(std::move(_shaders)) {}

private:
    void updateMeshBuf(const Mesh &) {};
    void draw() const {};
};

struct ProgramTest : public Program
{
    struct Vert
    {
        Vec3 pos;
    };

    ProgramTest();

    /* updateMeshBuf() creates a new meshBuf */
    void updateMeshBuf(const Mesh &mesh);

    /* draw() takes an existing meshBuf and renders it */
    void draw() const;

    GLuint vaoId, vertBufId, idxBufId;
    std::vector<Vert> vertBuf;
    std::vector<U32> idxBuf;
};

#endif

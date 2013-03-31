#ifndef DEMOLIB_PROGRAM_H
#define DEMOLIB_PROGRAM_H

#include "mesh.h"
#include <GL/gl.h>
#include <vector>
#include <string>
#include <memory>

struct Texture
{
    Texture();
    ~Texture();

    virtual void update() = 0;

    GLuint id;
};

struct Texture2DBase : public Texture
{
    Texture2DBase(unsigned _w, unsigned _h) :
        w(_w), h(_h)
    {}
    unsigned w, h;
};

template <typename T>
struct Texture2D : public Texture2DBase
{
    Texture2D(unsigned _w, unsigned _h, T *_data);

    void update();

    T *data;

    static const GLenum pixelType;
};


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
    virtual void draw() const = 0;

    GLuint id;

private:
    std::vector<Shader> shaders;
};

struct ProgramMesh : public Program
{
    struct Vert
    {
        Vec3 pos;
    };

    ProgramMesh(std::initializer_list<Shader> &&_shaders) :
        Program(std::move(_shaders)) {}

    virtual void updateMeshBuf(const Mesh &mesh);
    virtual void draw() const;
    virtual void meshDraw() const;

    GLuint vaoId, vertBufId, idxBufId;
    std::vector<Vert> vertBuf;
    std::vector<U32> idxBuf;
};

struct ProgramMeshWire : public ProgramMesh
{
    ProgramMeshWire(std::initializer_list<Shader> &&_shaders) :
        ProgramMesh(std::move(_shaders)) {}
    virtual void meshDraw() const;
};

struct ProgramRaw : public ProgramMesh
{
    ProgramRaw(std::initializer_list<Shader> &&_shaders) :
        ProgramMesh(std::move(_shaders)) {}

private:
    void updateMeshBuf(const Mesh &) {};
    void draw() const {};
};

struct ProgramTexturedQuad : public ProgramMesh
{
    ProgramTexturedQuad(GLuint texUnit = 0);

    void updateTexture(const Texture2DBase &tex);
    void meshDraw() const;

    GLuint texId, texSampler, texUnit;
};

struct ProgramTest : public ProgramMesh
{
    ProgramTest();
};

#endif

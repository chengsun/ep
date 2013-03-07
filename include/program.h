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
    Texture2D(unsigned _w, unsigned _h,
              const std::shared_ptr<T> _data = std::shared_ptr<T>());

    void update();

    const std::shared_ptr<T> data;

    static const GLenum pixelType;
};

template <typename T>
Texture2D<T>::Texture2D(unsigned _w, unsigned _h,
                     const std::shared_ptr<T> _data) :
    Texture2DBase(_w, _h),
    data(_data ? _data :
         std::shared_ptr<T>
             (new T[w*h], std::default_delete<T[]>()))
{
}

template <typename T>
void Texture2D<T>::update()
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, pixelType, data.get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}



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

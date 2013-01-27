#ifndef DEMOLIB_PROGRAM_H
#define DEMOLIB_PROGRAM_H

#include "mesh.h"
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

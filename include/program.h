#ifndef DEMOLIB_PROGRAM_H
#define DEMOLIB_PROGRAM_H

#include "mesh.h"
#include <GL/gl.h>
#include <vector>
#include <string>
#include <memory>

struct TextureBase
{
    TextureBase();
    ~TextureBase();

    virtual void bind(GLuint texUnit) const = 0;
    virtual void update(GLuint texUnit) = 0;

    GLuint id;
};

struct Texture2DBase : public TextureBase
{
    Texture2DBase(int _w, int _h, int _stride) : w(_w), h(_h), stride(_stride) {}

    int w, h;
    int stride;

    void bind(GLuint texUnit) const;
    void unbind() const;
};

template <typename T>
struct Texture2D : public Texture2DBase
{
    Texture2D(int _w = -1, int _h = -1, T *_data = nullptr, int _stride = 0);
    T *data;
    bool allocated;         // if glTexImage2D already called (via allocate())
                            // to allocate memory on the GPU

    /* must be called once after init */
    virtual void allocate(GLuint texUnit);
    /* called every time texture is to change */
    virtual void update(GLuint texUnit);

    static const GLenum pixelType;
};


struct Shader
{
public:
    Shader(GLenum _type, const char *_name);
    ~Shader();

    bool compile();
    bool compileLazy() {
        if (id == -1u) return compile();
        return true;
    }

    GLuint id;
    GLenum type;
    const char *name, *strType;
    std::string code;

    static std::shared_ptr<Shader> FromFile(GLenum type, const char *filename);
    static std::shared_ptr<Shader> Inline(GLenum type, std::string shaderCode, const char *name = "<inline>");
};

struct Program
{
    static constexpr U32 PrimitiveRestartIndex = 0xFFFFFFFF;

protected:
    Program(std::initializer_list<std::shared_ptr<Shader> > &&_shaders);

public:
    virtual void draw() const = 0;
    static GLuint doLink(std::vector<std::shared_ptr<Shader> > &shaders);
    bool link();

    /* overloadable use functions */
    virtual void useId(GLuint id);
    virtual void unuse();
    /* convenience */
    void use() {useId(id);}
    /* checks */
    static bool isInUseId(GLuint id);
    bool isInUse() const {return isInUseId(id);}

    GLint uniformLocation(const char *name);
    bool setUniform(const char *name, GLint v);
    bool setUniform(const char *name, float v);
    bool setUniform(const char *name, const glm::vec2 &v);
    bool setUniform(const char *name, const glm::vec3 &v);
    bool setUniform(const char *name, const glm::vec4 &v);
    bool setUniform(const char *name, const glm::mat2 &v, bool trans=false);
    bool setUniform(const char *name, const glm::mat3 &v, bool trans=false);
    bool setUniform(const char *name, const glm::mat4 &v, bool trans=false);


    GLuint id;

protected:
    /* called once link succeeds -- set up uniforms here */
    virtual void postLink() {}

private:
    std::vector<std::shared_ptr<Shader> > shaders;
};

struct VertexAttribDescriptor
{
    const char *shaderAttrName;
    unsigned offset;
};

struct VertBuf
{
    glm::vec3 pos;
    glm::vec2 texCoord;
    VertBuf &operator=(const MeshVert &vert) {
        pos = vert.pos;
        texCoord = vert.texCoord;
        return (*this);
    }
};

struct ProgramMesh : public Program
{
    ProgramMesh(std::initializer_list<std::shared_ptr<Shader> > &&_shaders) :
        Program(std::move(_shaders)) {}

    virtual void useId(GLuint id);
    virtual void unuse();

    virtual void updateMeshBuf(const Mesh &mesh);
    /* draw after doing sanity checks that this program is being used */
    virtual void draw() const;
    virtual void drawWire() const;
    /* example:
     * prog->link();
     * ...
     * prog->use();
     * prog->draw();
     * prog->unuse();
     */

    GLuint vaoId, vertBufId, idxBufId;
    std::vector<VertBuf> vertBuf;
    std::vector<U32> idxBuf;

    /* debug programs */
    static const std::shared_ptr<Shader> vsDebug, fsDebugFace, fsDebugEdge;
    static GLuint debugFaceId, debugEdgeId;

    /* ensure debug programs are used */
    void debugLink();
    /* draw without doing checks */
    void debugDraw() const;
    void debugDrawWire() const;
    /* debug example:
     * prog->debugLink();
     * ...
     * prog->use(prog->debugFaceId);
     * prog->debugDraw();
     * prog->unuse();
     */

private:
    void doDraw(GLenum mode) const;
};

/*
struct ProgramMeshDebug : public ProgramMesh
{
    ProgramMeshDebug();
    virtual void draw() const;
};
*/

struct ProgramRaw : public ProgramMesh
{
    ProgramRaw(std::initializer_list<std::shared_ptr<Shader> > &&_shaders) :
        ProgramMesh(std::move(_shaders)) {}

private:
    void updateMeshBuf(const Mesh &) {};
    void draw() const {};
};

struct ProgramTexturedQuad : public ProgramMesh
{
    ProgramTexturedQuad(GLuint texUnit,
            std::initializer_list<std::shared_ptr<Shader> > &&_shaders = {vs, fs});

    GLuint texUnit;
    static const std::shared_ptr<Shader> vs, fs;

protected:
    virtual void postLink();
};



struct FBBase
{
};

struct FBDefault : public FBBase
{
};

struct FBO : public FBBase
{
    const int w, h;
    GLuint id, depthBufId;

    enum ConstructionFlags {
        DEPTHBUF = 1,
    };

    FBO(int _w, int _h, int flags = 0);
    virtual ~FBO();
    void bind();
    void unbind();

    /* bind a texture to draw to */
    void bindTexture(const Texture2DBase &tex, int attachment=0);
};

struct GLContext
{
    static GLContext &get()
    {
        static GLContext instance;
        return instance;
    }

    void bindFBO(const FBO &fbo);
    void unbindFBO();

    FBDefault defaultFB;
    FBBase *curFBO;
private:
    GLContext() : curFBO(0) {}
    GLContext(GLContext const&);
    void operator=(GLContext const&);
};



#endif

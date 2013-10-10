#ifndef DEMOLIB_PROGRAM_H
#define DEMOLIB_PROGRAM_H

#include "mesh.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <string>

struct TextureBase
{
    TextureBase();
    ~TextureBase();

    bool allocated;         // if glTexImage#D already called (via allocate())
                            // to allocate memory on the GPU

    // bind texture without updating
    virtual void bind(GLuint texUnit) const = 0;
    // allocate and update texture (only called once), needs to be bound first
    virtual void allocate() = 0;
    // update texture, needs to be bound first
    virtual void update() = 0;
    // get texture from GPU
    virtual void updateLocal() = 0;

    static bool isInUseId(GLuint id) {return currentId() == id;}
    bool isInUse() const {return isInUseId(id);}
    static GLuint currentId();

    GLuint id;
};

struct Texture2DBase : public TextureBase
{
    Texture2DBase(int _w, int _h, int _stride) : w(_w), h(_h), stride(_stride) {}

    int w, h;
    int stride;     // can be set to 0

    void bind(GLuint texUnit) const;
    void unbind() const;
};

/* 2D texture with optionally automatically managed memory */
template <typename T>
struct Texture2D : public Texture2DBase
{
    /* pass all defaults to not initialise anything:
     *      autoManaged is set to false, nothing done on destruction
     * pass width/height but not data
     *      autoManaged is set to true, data is automatically allocated and
     *      destroyed
     * pass width height and data
     *      autoManaged is set to false
     */
    // constructor to do nothing: manually set everything
    Texture2D() :
        Texture2DBase(-1, -1, 0), data(nullptr),
        autoManaged(false)
    {}
    // constructor to auto-manage memory: manually set nothing
    Texture2D(int _w, int _h) :
        Texture2DBase(_w, _h, 0), data(new T[_w*_h]),
        autoManaged(true)
    {}
    // constructor to manually-manage memory from an external source:
    // memory will not be deallocated on destruction
    Texture2D(int _w, int _h, T *_data, int _stride = 0) :
        Texture2DBase(_w, _h, _stride), data(_data),
        autoManaged(false)
    {}

    virtual ~Texture2D()
    {
        if (autoManaged) {
            delete[] data;
        }
    }
    T *const data;
    const bool autoManaged; // if we need to deallocate memory on destruction

    virtual void allocate();
    virtual void update();
    virtual void updateLocal();

    static const GLenum pixelType, pixelFormat, internalPixelFormat;
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

    virtual void use();
    virtual void unuse();
    /* checks */
    static GLuint currentId();
    static bool isInUseId(GLuint id) {return currentId() == id;}
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

struct ProgramRaw : public Program
{
    ProgramRaw(std::initializer_list<std::shared_ptr<Shader> > &&_shaders) :
        Program(std::move(_shaders)) {}

private:
    void draw() const {};
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

    virtual void use();
    virtual void unuse();

    /* accepts vertices iff (vert.mask | vertMask) == 0xFF */
    virtual void updateMeshBuf(const Mesh &mesh, uint8_t vertMask = 0xFF, uint8_t faceMask = 0xFF);
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

    static const std::shared_ptr<Shader> vs;
private:
    void doDraw(GLenum mode) const;
};

struct ProgramMeshDebugVisFace : public ProgramMesh
{
    ProgramMeshDebugVisFace() :
        ProgramMesh({vs, fs})
    {}
    static const std::shared_ptr<Shader> fs;
};

struct ProgramMeshDebugVisEdge : public ProgramMesh
{
    ProgramMeshDebugVisEdge() :
        ProgramMesh({vs, fs})
    {}
    static const std::shared_ptr<Shader> fs;
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

struct ProgramTexQuadImp : public Program
{
    ProgramTexQuadImp(GLuint texUnit,
            std::initializer_list<std::shared_ptr<Shader> > &&_shaders = {vs, fs});

    virtual void draw() const;

    GLuint texUnit, vaoId;
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

    // bind a texture to draw to
    void bindTexture(const Texture2DBase &tex, int attachment);
};

struct GLContext
{
    static GLContext &get()
    {
        static GLContext instance;
        return instance;
    }

    void bindFBO(FBO &fbo);
    void unbindFBO();

    FBDefault defaultFB;
    FBBase *curFBO;
private:
    GLContext() : curFBO(0) {}
    GLContext(GLContext const&);
    void operator=(GLContext const&);
};



#endif

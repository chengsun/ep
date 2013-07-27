#include "demolib.h"
#include <cstdio>

TextureBase::TextureBase()
{
    glGenTextures(1, &id);
    LOG("Created texture %d", id);
}

TextureBase::~TextureBase()
{
    glDeleteTextures(1, &id);
}

void Texture2DBase::bind(GLuint texUnit) const
{
    ASSERTX(id != -1u, "Binding invalid texture");
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2DBase::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

template <typename T>
void Texture2D<T>::allocate(GLuint texUnit)
{
    if (allocated) {
        LOG("WARNING: Allocate on already allocated texture %d", id);
    }
    bind(texUnit);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, pixelType, data);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    unbind();
    allocated = true;
}

template <typename T>
void Texture2D<T>::update(GLuint texUnit)
{
    ASSERTX(allocated, "Updating unallocated texture %d", id);
    bind(texUnit);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, pixelType, data);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

template <> const GLenum Texture2D<uint8_t>::pixelType = GL_UNSIGNED_BYTE;
template <> const GLenum Texture2D<float>::pixelType = GL_FLOAT;

template struct Texture2D<uint8_t>;
template struct Texture2D<float>;



Shader::Shader(GLenum _type, const char *_name) :
    id(-1u),
    type(_type),
    name(_name)
{
    switch (type) {
        case GL_VERTEX_SHADER:   strType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strType = "fragment"; break;
        default:                 strType = "unknown";
    }
}

std::shared_ptr<Shader> Shader::FromFile(GLenum type, const char *filename)
{
    std::shared_ptr<Shader> shader = std::make_shared<Shader>(type, filename);

    LOG("Opening %s shader program \"%s\"", shader->strType, filename);

    FILE *file = fopen(filename, "rb");
    ASSERTX(file, "Failed to open %s shader program \"%s\"", shader->strType,
            filename);
    fseek(file, 0, SEEK_END);
    unsigned long fileLength = ftell(file);
    rewind(file);

    char *shaderCode = new char[fileLength];
    int shaderReadLength = fread(shaderCode, 1, fileLength, file);
    fclose(file);

    shader->code = std::string(shaderCode, shaderReadLength);
    delete[] shaderCode;

    return shader;
}

std::shared_ptr<Shader> Shader::Inline(GLenum type, std::string shaderCode, const char *name)
{
    std::shared_ptr<Shader> shader = std::make_shared<Shader>(type, name);
    shader->code = shaderCode;
    return shader;
}

bool Shader::compile()
{
    LOG("Compiling %s shader program \"%s\"", strType, name);

    id = glCreateShader(type);
    const char *codeStr = code.data();
    int codeLen = code.size();
    glShaderSource(id, 1, &codeStr, &codeLen);

    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);

        ASSERTX(false, "Shader compile failure in %s shader \"%s\":\n%s",
            strType, name, strInfoLog);
        delete[] strInfoLog;
        return false;
    }

    return true;
}

Shader::~Shader()
{
    glDeleteShader(id);
}


constexpr U32 Program::PrimitiveRestartIndex;

Program::Program(std::initializer_list<std::shared_ptr<Shader> > &&_shaders) :
    id(-1u),
    shaders(_shaders)
{
}

void Program::useId(GLuint id)
{
    glUseProgram(id);
}
void Program::unuse()
{
    ASSERTX(isInUse(), "Program not being used");
    glUseProgram(0);
}
bool Program::isInUseId(GLuint id)
{
    GLint curId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curId);
    return curId == static_cast<GLint> (id);
}

GLuint Program::doLink(std::vector<std::shared_ptr<Shader> > &shaders)
{
    for (auto &shader : shaders) {
        shader->compileLazy();
    }

    LOG("Linking shader program");
    GLuint id = glCreateProgram();
    for (auto const &shader : shaders) {
        glAttachShader(id, shader->id);
    }
    glLinkProgram(id);

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(id, infoLogLength, NULL, strInfoLog);
        ASSERTX(false, "Shader linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
        return -1u;
    }

    for (auto const &shader : shaders) {
        glDetachShader(id, shader->id);
    }

    return id;
}

bool Program::link()
{
    id = doLink(shaders);
    if (id == -1u) return false;

    postLink();
    return true;
}

GLint Program::uniformLocation(const char *name)
{
    return glGetUniformLocation(id, name);
}

bool Program::setUniform(const char *name, GLint v)
{
    GLint loc = uniformLocation(name);
    glUniform1i(loc, v);
    return loc != -1;
}
bool Program::setUniform(const char *name, float v)
{
    GLint loc = uniformLocation(name);
    glUniform1f(loc, v);
    return loc != -1;
}
bool Program::setUniform(const char *name, const glm::vec2 &v)
{
    GLint loc = uniformLocation(name);
    glUniform2fv(loc, 1, glm::value_ptr(v));
    return loc != -1;
}
bool Program::setUniform(const char *name, const glm::vec3 &v)
{
    GLint loc = uniformLocation(name);
    glUniform3fv(loc, 1, glm::value_ptr(v));
    return loc != -1;
}
bool Program::setUniform(const char *name, const glm::vec4 &v)
{
    GLint loc = uniformLocation(name);
    glUniform4fv(loc, 1, glm::value_ptr(v));
    return loc != -1;
}
bool Program::setUniform(const char *name, const glm::mat2 &v, bool trans)
{
    GLint loc = uniformLocation(name);
    glUniformMatrix2fv(uniformLocation(name), 1, trans, glm::value_ptr(v));
    return loc != -1;
}
bool Program::setUniform(const char *name, const glm::mat3 &v, bool trans)
{
    GLint loc = uniformLocation(name);
    glUniformMatrix3fv(uniformLocation(name), 1, trans, glm::value_ptr(v));
    return loc != -1;
}
bool Program::setUniform(const char *name, const glm::mat4 &v, bool trans)
{
    GLint loc = uniformLocation(name);
    glUniformMatrix4fv(uniformLocation(name), 1, trans, glm::value_ptr(v));
    return loc != -1;
}

const std::shared_ptr<Shader> ProgramMesh::vsDebug =
    Shader::Inline(GL_VERTEX_SHADER, R"(
        #version 130
        in vec4 pos;
        in vec2 texCoord;
        uniform mat4 transform;
        out vec2 fTexCoord;
        void main()
        {
            gl_Position = transform * pos;
            fTexCoord = texCoord;
        }
    )", "ProgramMesh::vsDebug");
const std::shared_ptr<Shader> ProgramMesh::fsDebugFace =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        in vec2 fTexCoord;
        void main()
        {
            gl_FragColor = vec4(fTexCoord, 0.f, 1.f);
        }
    )", "ProgramMesh::fsDebugFace");
const std::shared_ptr<Shader> ProgramMesh::fsDebugEdge =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        in vec2 fTexCoord;
        void main()
        {
            gl_FragColor = vec4(1.f, 1.f, 1.f, 1.f);
        }
    )", "ProgramMesh::fsDebugEdge");


void ProgramMesh::debugLink()
{
    std::vector<std::shared_ptr<Shader> >
            shadersDebugFace = {vsDebug, fsDebugFace},
            shadersDebugEdge = {vsDebug, fsDebugEdge};
    if (debugFaceId == -1u) {
        debugFaceId = doLink(shadersDebugFace);
        ASSERTX(debugFaceId != -1u, "Failed to compile debug face program");
    }
    if (debugEdgeId == -1u) {
        debugEdgeId = doLink(shadersDebugEdge);
        ASSERTX(debugEdgeId != -1u, "Failed to compile debug edge program");
    }
}

void ProgramMesh::useId(GLuint id)
{
    Program::useId(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);
}
void ProgramMesh::unuse()
{
    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
    Program::unuse();
}

void ProgramMesh::doDraw(GLenum mode) const
{
    glBindVertexArray(vaoId);
    glDrawElements(mode, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ProgramMesh::draw() const
{
    ASSERTX(id != -1u, "Shader program not compiled");
    ASSERTX(isInUse(), "Program not in use");

    doDraw(GL_TRIANGLE_FAN);
}
void ProgramMesh::drawWire() const
{
    ASSERTX(id != -1u, "Shader program not compiled");
    ASSERTX(isInUse(), "Program not in use");

    doDraw(GL_LINE_LOOP);
}

GLuint ProgramMesh::debugFaceId = -1u;
GLuint ProgramMesh::debugEdgeId = -1u;

void ProgramMesh::debugDraw() const
{
    doDraw(GL_TRIANGLE_FAN);
}
void ProgramMesh::debugDrawWire() const
{
    doDraw(GL_LINE_LOOP);
}

void ProgramMesh::updateMeshBuf(const Mesh &mesh)
{
    vertBuf.resize(mesh.verts.size());
    for (U32 vertIdx = 0; vertIdx < mesh.verts.size(); vertIdx++) {
        vertBuf[vertIdx] = mesh.verts[vertIdx];
    }

    idxBuf.clear();
    for (U32 faceIdx = 0; faceIdx < mesh.faces.size(); faceIdx++) {
        const MeshFace &face = mesh.faces[faceIdx];
        if (face.material == 0xFF) continue;
        if (!idxBuf.empty()) {
            idxBuf.push_back(PrimitiveRestartIndex);
        }
        for (U32 slotIdx = 0; slotIdx < face.count; slotIdx++) {
            idxBuf.push_back(face.verts[slotIdx]);
        }
    }

    glGenBuffers(1, &vertBufId);
    glGenBuffers(1, &idxBufId);
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, vertBufId);
    glBufferData(GL_ARRAY_BUFFER, vertBuf.size() * sizeof(vertBuf[0]),
                 &vertBuf[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertBuf[0]), (GLvoid *) offsetof(VertBuf, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertBuf[0]), (GLvoid *) offsetof(VertBuf, texCoord));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBufId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBuf.size() * sizeof(idxBuf[0]),
                 &idxBuf[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}




const std::shared_ptr<Shader> ProgramTexturedQuad::vs =
    Shader::Inline(GL_VERTEX_SHADER, R"(
        #version 130
        in vec4 pos;
        in vec2 texCoord;
        uniform mat4 transform;
        out vec2 fTexCoord;
        void main()
        {
            gl_Position = transform*pos;
            fTexCoord = texCoord;
        }
    )", "ProgramTexturedQuad::vs");
const std::shared_ptr<Shader> ProgramTexturedQuad::fs =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        in vec2 fTexCoord;
        uniform sampler2D tex;
        void main()
        {
            gl_FragColor = vec4(texture(tex, fTexCoord).rrr, 1.0f);
        }
    )", "ProgramTexturedQuad::fs");

ProgramTexturedQuad::ProgramTexturedQuad(GLuint _texUnit,
            std::initializer_list<std::shared_ptr<Shader> > &&_shaders) :
    ProgramMesh(std::move(_shaders)),
    texUnit(_texUnit)
{
    std::unique_ptr<Mesh> mesh = Mesh::createRing(4, PI/4.f, sqrt(2.f));
    updateMeshBuf(*mesh);
}

void ProgramTexturedQuad::postLink()
{
    ProgramMesh::postLink();
    use();
    setUniform("tex", (GLint) texUnit);
    setUniform("gTransform", glm::mat4());
    unuse();
}


FBO::FBO(int _w, int _h, int flags) :
w(_w), h(_h), id(-1), depthBufId(-1)
{
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    if (flags & DEPTHBUF) {
        glGenRenderbuffers(1, &depthBufId);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBufId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                 w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                     GL_DEPTH_ATTACHMENT,
                                     GL_RENDERBUFFER, depthBufId);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FBO::~FBO()
{
    glDeleteFramebuffers(1, &id);
}
void FBO::bindTexture(const Texture2DBase &tex, int attachment)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachment,
                           GL_TEXTURE_2D, tex.id, 0);
}

void GLContext::bindFBO(const FBO &fbo)
{
    if (curFBO) unbindFBO();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
    ASSERTX(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Bind on incomplete framebuffer");
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, fbo.w, fbo.h);
}
void GLContext::unbindFBO()
{
    if (!curFBO) {
        LOG("WARNING: unbinding FBO when no FBO attached; ignoring");
        return;
    }
    glPopAttrib();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

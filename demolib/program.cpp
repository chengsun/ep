#include "demolib.h"
#include <cstdio>

GLContext::GLContext() :
    curFBO(0)
{
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
void FBO::bindTexture(const TextureBase &tex, int attachment)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachment,
                           GL_TEXTURE_2D, tex.id, 0);
}

TextureBase::TextureBase()
{
    glGenTextures(1, &id);
    LOG("Created texture %d", id);
}

TextureBase::~TextureBase()
{
    glDeleteTextures(1, &id);
}


/*
template <typename T>
const GLenum Texture2D<T>::pixelType = -1;
*/

template <typename T>
void Texture2D<T>::update()
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, pixelType, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

        ASSERTX(false, "Shader compile failure in %s shader \"%s\": %s",
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

bool Program::link()
{
    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        shaders[shaderIdx]->compileLazy();

    LOG("Linking shader program");
    id = glCreateProgram();
    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glAttachShader(id, shaders[shaderIdx]->id);
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
        return false;
    }

    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glDetachShader(id, shaders[shaderIdx]->id);
    return true;
}

const std::shared_ptr<Shader> ProgramMesh::vsDebug =
    Shader::Inline(GL_VERTEX_SHADER, R"(
        #version 130
        in vec4 position;
        void main()
        {
            gl_Position = position;
        }
    )", "ProgramMesh::vsDebug");
const std::shared_ptr<Shader> ProgramMesh::fsDebugFace =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        uniform sampler2D tex;
        uniform ivec2 viewSize;
        void main()
        {
            gl_FragColor = vec4(1.f, 1.f, 1.f, 1.f);
        }
    )", "ProgramMesh::fsDebugFace");
const std::shared_ptr<Shader> ProgramMesh::fsDebugEdge =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        uniform sampler2D tex;
        uniform ivec2 viewSize;
        void main()
        {
            gl_FragColor = vec4(1.f, 1.f, 1.f, 1.f);
        }
    )", "ProgramMesh::fsDebugEdge");

void ProgramMesh::draw() const
{
    ASSERTX(id != -1u, "Shader program not compiled");
    glUseProgram(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLE_FAN, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
    glUseProgram(0);
}
void ProgramMesh::drawWire() const
{
    ASSERTX(id != -1u, "Shader program not compiled");
    glUseProgram(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);

    glBindVertexArray(vaoId);
    glDrawElements(GL_LINE_LOOP, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
    glUseProgram(0);
}

void ProgramMesh::updateMeshBuf(const Mesh &mesh)
{
    vertBuf.resize(mesh.verts.size());
    for (U32 vertIdx = 0; vertIdx < mesh.verts.size(); vertIdx++) {
        vertBuf[vertIdx].pos = mesh.verts[vertIdx].pos;
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBufId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBuf.size() * sizeof(idxBuf[0]),
                 &idxBuf[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

const std::shared_ptr<Shader> ProgramTexturedQuad::vs =
    Shader::Inline(GL_VERTEX_SHADER, R"(
        #version 130
        in vec4 position;
        void main()
        {
            gl_Position = position;
        }
    )", "ProgramTexturedQuad::vs");
const std::shared_ptr<Shader> ProgramTexturedQuad::fs =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        uniform sampler2D tex;
        uniform ivec2 viewSize;
        void main()
        {
            gl_FragColor = texture(tex, gl_FragCoord.xy/viewSize).rrra;
            //gl_FragColor = vec4((gl_FragCoord.x+50.f)/400.f, 0.f, 0.f, 1.f);
        }
    )", "ProgramTexturedQuad::fs");

ProgramTexturedQuad::ProgramTexturedQuad(GLuint _texUnit) :
    ProgramMesh({vs, fs}),
    texId(0), texSampler(0), texUnit(_texUnit)
{
    updateMeshBuf(*Mesh::createRing(4, PI/4.f, 2.f));

    GLuint texUnif = glGetUniformLocation(id, "tex");
    GLuint viewSizeUnif = glGetUniformLocation(id, "viewSize");
    glUseProgram(id);
    glUniform1i(texUnif, texUnit);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform2i(viewSizeUnif, viewport[2], viewport[3]);

    glUseProgram(0);

    glGenSamplers(1, &texSampler);
    glSamplerParameteri(texSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(texSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void ProgramTexturedQuad::setTexture(const Texture2DBase &tex)
{
    texId = tex.id;
}

void ProgramTexturedQuad::draw() const
{
    glUseProgram(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);

    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texId);
    glBindSampler(texUnit, texSampler);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLE_FAN, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindSampler(texUnit, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
    glUseProgram(0);
}

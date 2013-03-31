#include "demolib.h"
#include <cstdio>

Texture::Texture()
{
    glGenTextures(1, &id);
    LOG("Created texture %d", id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}


/*
template <typename T>
const GLenum Texture2D<T>::pixelType = -1;
*/
template <typename T>
Texture2D<T>::Texture2D(unsigned _w, unsigned _h, T *_data) :
    Texture2DBase(_w, _h),
    data(_data)
{
}

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

Shader Shader::FromFile(GLenum type, const char *filename)
{
    Shader shader(type, filename);

    LOG("Opening %s shader program \"%s\"", shader.strType, filename);

    FILE *file = fopen(filename, "rb");
    ASSERTX(file, "Failed to open %s shader program \"%s\"", shader.strType,
            filename);
    fseek(file, 0, SEEK_END);
    unsigned long fileLength = ftell(file);
    rewind(file);

    char *shaderCode = new char[fileLength];
    int shaderReadLength = fread(shaderCode, 1, fileLength, file);
    fclose(file);

    shader.compile(std::string(shaderCode, shaderReadLength));
    delete[] shaderCode;

    return shader;
}

Shader Shader::Inline(GLenum type, std::string shaderCode, const char *name)
{
    Shader shader(type, name);
    shader.compile(shaderCode);
    return shader;
}

bool Shader::compile(std::string shaderCode)
{
    LOG("Compiling %s shader program \"%s\"", strType, name);

    id = glCreateShader(type);
    const char *codeStr = shaderCode.data();
    int codeLen = shaderCode.size();
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

Program::Program(std::initializer_list<Shader> &&_shaders) :
    shaders(_shaders)
{
    LOG("Linking shader program");

    id = glCreateProgram();

    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glAttachShader(id, shaders[shaderIdx].id);

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
    }

    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glDetachShader(id, shaders[shaderIdx].id);
}

void ProgramMesh::draw() const
{
    glUseProgram(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);

    meshDraw();

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

void ProgramMesh::meshDraw() const
{
    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLE_FAN, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ProgramMeshWire::meshDraw() const
{
    glBindVertexArray(vaoId);
    glDrawElements(GL_LINE_LOOP, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

ProgramTexturedQuad::ProgramTexturedQuad(GLuint _texUnit) :
    ProgramMesh({
        Shader::Inline(GL_VERTEX_SHADER, R"(
            #version 130
            in vec4 position;
            void main()
            {
                gl_Position = position;
            }
        )"),
        Shader::Inline(GL_FRAGMENT_SHADER, R"(
            #version 130
            uniform sampler2D tex;
            uniform ivec2 viewSize;
            void main()
            {
                gl_FragColor = texture(tex, gl_FragCoord.xy/viewSize).rrra;
                //gl_FragColor = vec4((gl_FragCoord.x+50.f)/400.f, 0.f, 0.f, 1.f);
            }
        )")
    }),
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

void ProgramTexturedQuad::updateTexture(const Texture2DBase &tex)
{
    texId = tex.id;
}

void ProgramTexturedQuad::meshDraw() const
{
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texId);
    glBindSampler(texUnit, texSampler);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLE_FAN, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindSampler(texUnit, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#include "demolib.h"
#include <cstdio>

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

ProgramTest::ProgramTest() :
    Program({
        Shader::FromFile(GL_VERTEX_SHADER, "data/test.vs"),
        Shader::FromFile(GL_FRAGMENT_SHADER, "data/test.fs")
    })
{
}

void ProgramTest::updateMeshBuf(const Mesh &mesh)
{
    vertBuf.resize(mesh.verts.size());
    for (U32 vertIdx = 0; vertIdx < mesh.verts.size(); vertIdx++) {
        vertBuf[vertIdx].pos = mesh.verts[vertIdx].pos;
    }

    idxBuf.clear();
    for (U32 faceIdx = 0; faceIdx < mesh.faces.size(); faceIdx++) {
        if (faceIdx > 0) {
            idxBuf.push_back(PrimitiveRestartIndex);
        }
        const MeshFace &face = mesh.faces[faceIdx];
        for (U32 slotIdx = 0; slotIdx < face.count; slotIdx++) {
            idxBuf.push_back(face.verts[slotIdx]);
        }
    }

    glGenBuffers(1, &vertBufId);
    glGenBuffers(1, &idxBufId);

    glBindBuffer(GL_ARRAY_BUFFER, vertBufId);
    glBufferData(GL_ARRAY_BUFFER, vertBuf.size() * sizeof(vertBuf[0]),
                 &vertBuf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBufId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBuf.size() * sizeof(idxBuf[0]),
                 &idxBuf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, vertBufId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBufId);

    glBindVertexArray(0);
}

void ProgramTest::draw() const
{
    glUseProgram(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLE_FAN, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
    glUseProgram(0);
}

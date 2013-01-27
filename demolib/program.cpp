#include "demolib.h"
#include <cstdio>

Shader::Shader(GLenum _type, const char *_filename) :
    type(_type),
    filename(_filename)
{
    const char *strType = "unknown";
    switch (type) {
        case GL_VERTEX_SHADER: strType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strType = "fragment"; break;
    }

    LOG("Opening %s shader program \"%s\"", strType, filename);

    FILE *file = fopen(filename, "rb");
    ASSERTX(file, "Failed to open %s shader program \"%s\"", strType, filename);
    fseek(file, 0, SEEK_END);
    unsigned long fileLength = ftell(file);
    rewind(file);

    GLchar *shaderCode = new GLchar[fileLength];
    int shaderReadLength = fread(shaderCode, 1, fileLength, file);
    fclose(file);

    LOG("Compiling %s shader program \"%s\"", strType, filename);

    id= glCreateShader(type);
    glShaderSource(id, 1, const_cast<const GLchar **>(&shaderCode), &shaderReadLength);

    delete[] shaderCode;

    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);

        LOG("WARNING: Shader compile failure in %s shader \"%s\": %s",
            strType, filename, strInfoLog);
        delete[] strInfoLog;
    }
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
        LOG("WARNING: Shader linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (unsigned shaderIdx = 0; shaderIdx < shaders.size(); shaderIdx++)
        glDetachShader(id, shaders[shaderIdx].id);
}

ProgramTest::ProgramTest() :
    Program({
        Shader(GL_VERTEX_SHADER, "data/test.vs"),
        Shader(GL_FRAGMENT_SHADER, "data/test.fs")
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
}

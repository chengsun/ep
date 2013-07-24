#include "demo.h"
#include "mesh.h"

struct ProgramTest : public ProgramMesh<VertBufPT>
{
    ProgramTest();
};

ProgramTest::ProgramTest() :
    ProgramMesh({
        Shader::Inline(GL_VERTEX_SHADER, R"(
            #version 130
            in vec4 position;
            in vec2 texCoords;
            uniform mat4 gTransform;
            out vec2 textTexCoords;
            void main()
            {
                gl_Position = gTransform*position;
                textTexCoords = texCoords;
            }
        )"),
        Shader::Inline(GL_FRAGMENT_SHADER, R"(
            #version 130
            in vec2 textTexCoords;
            uniform sampler2D textTexCoords;
            void main()
            {
               gl_FragColor = vec4(texture(textTex, textTexCoords).rrr, 0.5f);
            }
        )")
    })
{
}

std::unique_ptr<Mesh> mesh;
ProgramTest *program;
TextureTextSDF *textTex;

void demo_init(unsigned, unsigned)
{
    program = new ProgramTest;
    program->link();

    /*
    mesh = Mesh::createRing(4, PI/4.f);
    mesh->splitVert(mesh->eEdge(0,0), mesh->eVertPrev(0,0));
    mesh->splitVert(mesh->eEdge(0,2), mesh->eVertPrev(0,2));
    mesh->splitVert(mesh->eEdge(0,4), mesh->eVertPrev(0,4));
    mesh->splitVert(mesh->eEdge(0,6), mesh->eVertPrev(0,6));
    mesh->verts[4].pos.x = 0;
    mesh->verts[6].pos.x = 0;
    mesh->verts[5].pos.y = 0;
    mesh->verts[7].pos.y = 0;
    U32 edge = mesh->splitFace(mesh->eEdge(0,1), mesh->eEdge(0,5));
    mesh->splitVert(edge, mesh->eVertPrev(edge));
    mesh->debugOut();
    ASSERTX(mesh->check());
    */

    mesh = Mesh::createGrid(3, 2);
    mesh->debugOut();
    ASSERTX(mesh->check());

    program->updateMeshBuf(*mesh);
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    static float t = 0.f;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    program->use();
    program->setUniform("gTransform", glm::rotate(glm::mat4(), t+=2.f, glm::vec3(0.f,1.f,0.f)));
    program->draw();
    program->unuse();
}

void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}

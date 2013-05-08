#include "demo.h"
#include "mesh.h"

struct ProgramTest : public ProgramMesh
{
    ProgramTest();
};

ProgramTest::ProgramTest() :
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
            void main()
            {
               gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
            }
        )")
    })
{
}

std::unique_ptr<Mesh> mesh;
ProgramTest *program;

void demo_init(unsigned, unsigned)
{
    program = new ProgramTest;
    program->link();

    mesh = Mesh::createRing(4, PI/4.f);
    mesh->splitVert(mesh->eEdge(0,0), mesh->eVertPrev(0,0));
    mesh->splitVert(mesh->eEdge(0,2), mesh->eVertPrev(0,2));
    mesh->splitVert(mesh->eEdge(0,4), mesh->eVertPrev(0,4));
    mesh->splitVert(mesh->eEdge(0,6), mesh->eVertPrev(0,6));
    U32 edge = mesh->splitFace(mesh->eEdge(0,1), mesh->eEdge(0,5));
    ASSERTX(edge/8 == 2 && mesh->eOpposite(edge)/8 == 0);
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    program->drawWire();
}

void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}

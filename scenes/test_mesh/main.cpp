#include "demo.h"
#include "mesh.h"

Mesh *mesh;
Program *program;

void demo_init(unsigned w, unsigned h)
{
    program = new Program({
        Shader(GL_VERTEX_SHADER, "data/mesh.vs"),
        Shader(GL_FRAGMENT_SHADER, "data/mesh.fs")
    });

    mesh = Mesh::createRing(4);
    mesh->splitVert(mesh->eEdge(0,0), mesh->eVertPrev(0,0));
    mesh->splitVert(mesh->eEdge(0,2), mesh->eVertPrev(0,2));
    mesh->splitVert(mesh->eEdge(0,4), mesh->eVertPrev(0,4));
    mesh->splitVert(mesh->eEdge(0,6), mesh->eVertPrev(0,6));
    U32 edge = mesh->splitFace(mesh->eEdge(0,1), mesh->eEdge(0,5));
    ASSERTX(edge/8 == 2 && mesh->eOpposite(edge)/8 == 0);
    mesh->debugOut();
    ASSERTX(mesh->check());
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program->id);

    MeshBuf buf(*mesh);
    buf.draw();
}

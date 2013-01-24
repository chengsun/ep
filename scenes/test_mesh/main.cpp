#include "demo.h"
#include "mesh.h"

void demo_init(unsigned w, unsigned h)
{
    Mesh *mesh = Mesh::createRing(4);
    mesh->debugOut();
    ASSERTX(mesh->check());
    LOG("\n");
    mesh->splitVert(mesh->eEdge(0,0), mesh->eEdge(1,0));
    mesh->debugOut();
    ASSERTX(mesh->check());
}

bool demo_prepareFrame()
{
    return false;
}

void demo_drawFrame()
{
}

#include "demo.h"
#include "mesh.h"

void demo_init(unsigned w, unsigned h)
{
    Mesh *mesh = Mesh::createRing(4);
    mesh->debugOut();
    ASSERTX(mesh->check());
    LOG("\n");
    mesh->splitVert(mesh->eEdge(0,0), mesh->eVertPrev(0,0));
    mesh->splitVert(mesh->eEdge(0,2), mesh->eVertPrev(0,2));
    mesh->splitVert(mesh->eEdge(0,4), mesh->eVertPrev(0,4));
    mesh->splitVert(mesh->eEdge(0,6), mesh->eVertPrev(0,6));
    mesh->debugOut();
    ASSERTX(mesh->check());
    LOG("\n");
    U32 edge = mesh->splitFace(mesh->eEdge(0,1), mesh->eEdge(0,5));
    ASSERTX(edge/8 == 2 && mesh->eOpposite(edge)/8 == 0);
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

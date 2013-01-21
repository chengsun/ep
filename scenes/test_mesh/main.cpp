#include "demo.h"
#include "mesh.h"

void demo_init(unsigned w, unsigned h)
{
    Mesh *mesh = createRingMesh(4);
    meshDebugOut(mesh);
    ASSERTX(meshCheck(mesh));
    LOG("\n");
    meshSplitVert(mesh, mesh->edge(0,0), mesh->edge(1,0));
    meshDebugOut(mesh);
    ASSERTX(meshCheck(mesh));
}

bool demo_prepareFrame()
{
    return false;
}

void demo_drawFrame()
{
}

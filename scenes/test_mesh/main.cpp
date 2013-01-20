#include "demo.h"
#include "mesh.h"

void demo_init(unsigned w, unsigned h)
{
    Mesh *mesh = createRingMesh(4);
    for (int f = 0; f < 2; ++f) {
        for (int s = 0; s < mesh->faces[f].count; ++s) {
            U32 opp = mesh->eOpposite(f,s);
            U32 p = mesh->ePrev(f,s);
            LOG("%u,%u is %u and is opposite %u,%u and has previous %u,%u", f, s, mesh->vertIdx(f,s), opp/8,opp%8, p/8,p%8);
        }
    }
    ASSERTX(meshCheck(mesh));
    LOG("\n");
    meshSplitVert(mesh, mesh->vertIdx(1,0), mesh->vertIdx(0,0));
    for (int f = 0; f < 2; ++f) {
        for (int s = 0; s < mesh->faces[f].count; ++s) {
            U32 opp = mesh->eOpposite(f,s);
            U32 p = mesh->ePrev(f,s);
            LOG("%u,%u is %u and is opposite %u,%u and has previous %u,%u", f, s, mesh->vertIdx(f,s), opp/8,opp%8, p/8,p%8);
        }
    }
    ASSERTX(meshCheck(mesh));
}

bool demo_prepareFrame()
{
    return false;
}

void demo_drawFrame()
{
}

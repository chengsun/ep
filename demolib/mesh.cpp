#include "mesh.h"

Mesh *createRingMesh(unsigned sides, float radius)
{
    Mesh *mesh = new Mesh;
    mesh->verts.reserve(sides);
    for (unsigned i = 0; i < sides; i++) {
        MeshVert vert;
        mesh->verts.push_back(vert);
    }
    return mesh;
}

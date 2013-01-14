#ifndef MESH_H
#define MESH_H

#include "types.h"
#include <vector>

extern "C" {


static const unsigned MAXVERT = 8;   // max vertices per face

struct MeshVert
{
    Vec3 pos;
    Vec3 normal;
};

struct MeshFace
{
    U8 count;                   // number of verts/edges
    U8 material;                // material index, 0 for none

    Vec3 normal;
    U32 verts[MAXVERT];         // vertex indices
};

struct Mesh
{
    std::vector<MeshVert> verts;
    std::vector<MeshFace> faces;
};

Mesh *createRingMesh(unsigned sides);
void meshExtrude(Mesh *mesh, unsigned faceIdx, float length);


}

#endif

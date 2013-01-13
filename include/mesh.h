#ifndef MESH_H
#define MESH_H

#include "types.h"

extern "C" {


static const int MAXVERT = 8;   // max vertices per face

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

Mesh *createRingMesh(unsigned sides, float radius);
void meshExtrude(Mesh *mesh, float length);


}

#endif

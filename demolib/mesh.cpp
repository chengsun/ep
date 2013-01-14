#include "mesh.h"
#include "demolib.h"
#include <cmath>

Mesh *createRingMesh(unsigned sides)
{
    ASSERTX(sides <= MAXVERT, "createRingMesh with more than MAXVERT verts");

    Mesh *mesh = new Mesh;
    mesh->verts.resize(sides);

    mesh->faces.resize(2);
    mesh->faces[0].count = sides;
    mesh->faces[1].count = sides;
    mesh->faces[0].material = 0;
    mesh->faces[1].material = 0;
    mesh->faces[0].normal = {0.f, 0.f,  1.f};
    mesh->faces[1].normal = {0.f, 0.f, -1.f};

    float curRot = 0.f, incRot = 2*PI / sides;
    for (unsigned i = 0; i < sides; i++, curRot += incRot) {
        MeshVert &thisVert = mesh->verts[i];
        thisVert.pos = {cosf(curRot), sinf(curRot), 0.f};
        thisVert.normal = thisVert.pos;

        mesh->faces[0].verts[i] = i;
        mesh->faces[1].verts[i] = sides-i-1;
    }
    return mesh;
}

void meshExtrude(Mesh *mesh, unsigned faceIdx, float length)
{
    unsigned oldSize = mesh->faces.size();
    ASSERTX(faceIdx < oldSize, "meshExtrude invalid faceIdx");

    MeshFace &face = mesh->faces[faceIdx];
    mesh->faces.resize(oldSize + face.count);
    for (unsigned i = 0; i < face.count; i++) {
        mesh->verts[face.verts[i]].pos += length * face.normal;
        if (i+1 == face.count) continue;
        MeshFace &newFace = mesh->faces[oldSize + i];
        newFace.count = 4;
        newFace.material = face.material;
        // XXX: is this the right way of calculating the new face normals?
        newFace.normal = Vec3::cross(face.normal,
                                     mesh->verts[face.verts[i+1]].pos -
                                     mesh->verts[face.verts[i]].pos);
    }
}

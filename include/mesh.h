#ifndef MESH_H
#define MESH_H

#include "util.h"
#include "types.h"
#include <vector>

extern "C" {


static const unsigned MAXVERT = 8;   // max vertices per face

struct MeshVert
{
    U8 selected;
    U32 tmp;
    Vec3 pos;
    Vec3 normal;
};

struct MeshFace
{
    U8 count;                   // number of verts/edges
    U8 material;                // material index, 0 for none
    U8 selected;
    U32 tmp;

    Vec3 normal;
    U32 verts[MAXVERT];         // vertex indices
    U32 opposite[MAXVERT];      // half-edge which is opposite
};

struct Mesh
{
    std::vector<MeshVert> verts;
    std::vector<MeshFace> faces;

    inline U32 edge(U32 face, U32 slot) const {
        ASSERTX(face < faces.size());
        ASSERTX(slot < faces[face].count);

        return (face*8) + (slot);
    }

    inline bool eCheck(U32 face, U32 slot) const {
        if (face >= faces.size()) {
            LOG("eCheck failed for (face %u, slot %u): invalid face index",
                face, slot);
            return false;
        }
        if (slot >= faces[face].count) {
            LOG("eCheck failed for (face %u, slot %u): invalid slot index",
                face, slot);
            return false;
        }
        return true;
    }
    inline bool eCheck(U32 edge) const {
        return eCheck(edge/8, edge%8);
    }

    inline U32 ePrev(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        const U8 &faceCount = faces[face].count;
        return (face*8) + ((slot+faceCount-1) % faceCount);
    }
    inline U32 ePrev(U32 edge) const {
        return ePrev(edge/8, edge%8);
    }

    inline U32 eNext(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        const U8 &faceCount = faces[face].count;
        return (face*8) + ((slot+1) % faceCount);
    }
    inline U32 eNext(U32 edge) const {
        return eNext(edge/8, edge%8);
    }

    inline U32 eVertPrev(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        return eNext(eOpposite(face, slot));
    }
    inline U32 eVertPrev(U32 edge) const {
        ASSERTX(eCheck(edge));
        return eNext(eOpposite(edge));
    }

    inline U32 eVertNext(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        return eOpposite(ePrev(face, slot));
    }
    inline U32 eVertNext(U32 edge) const {
        ASSERTX(eCheck(edge));
        return eOpposite(ePrev(edge));
    }

    inline U32 &eOpposite(U32 face, U32 slot) {
        ASSERTX(eCheck(face, slot));
        return faces[face].opposite[slot];
    }
    inline U32 &eOpposite(U32 edge) {
        return eOpposite(edge/8, edge%8);
    }

    inline U32 eOpposite(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        return faces[face].opposite[slot];
    }
    inline U32 eOpposite(U32 edge) const {
        return eOpposite(edge/8, edge%8);
    }

    inline U32 &vertIdx(U32 face, U32 slot) {
        ASSERTX(eCheck(face, slot));
        return faces[face].verts[slot];
    }
    inline U32 &vertIdx(U32 edge) {
        return vertIdx(edge/8, edge%8);
    }

    inline U32 vertIdx(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        return faces[face].verts[slot];
    }
    inline U32 vertIdx(U32 edge) const {
        return vertIdx(edge/8, edge%8);
    }
};

struct MeshBuf
{
    std::vector<U32> idxBuf;
};


bool meshCheck(const Mesh *mesh);
bool meshCheckFlags(const Mesh *mesh);

void meshDebugOut(const Mesh *mesh);

Mesh *createRingMesh(unsigned sides);

void meshExtrudeFace(Mesh *mesh, U32 faceIdx, float length);
U32 meshSplitVert(Mesh *mesh, U32 beginEdge, U32 endEdge);


}

#endif

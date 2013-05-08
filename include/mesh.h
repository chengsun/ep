#ifndef MESH_H
#define MESH_H

#include "util.h"
#include "linmat.h"
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

    Mesh() {
    }

    Mesh(Mesh &&other) {
        verts = other.verts;
        faces = other.faces;
    }

    // basic operations

    static std::unique_ptr<Mesh> createRing(unsigned sides, float phase = 0.f, float radius = 1.f);
    static std::unique_ptr<Mesh> createGrid(int w, int h);

    bool check() const;
    /* meshCheckFlags checks a mesh to ensure that the selected flags are all
     * reset
     */
    bool checkFlags() const;

    void debugOut() const;

    void extrudeFace(U32 faceIdx, float length);

    /* dupVert (internally used) duplicates the slot before baseEdge
     * v0 ---e--> v1
     * dupVertex(e) creates a new edge e0 and a duplicate slot.
     * v0 --e0--> v0 ---e--> v1
     * the new value of e is returned. e0 is equal to baseEdge after the operation.
     */
    U32 dupVert(U32 baseEdge);

    /* splitVert performs a vertex split
     * beginEdge - specifies the begin half-edge (inclusive)
     * endEdge - specifies the end half-edge (exclusive)
     * note that the order of edge traversal is CLOCKWISE (i.e. eVertPrev)!
     * returns the half-edge representing the new vertex
     */
    U32 splitVert(U32 beginEdge, U32 endEdge);

    /* splitFace performs a face split
     * beginEdge - specifies the begin half-edge
     * endEdge - specifies the end half-edge
     * returns the half-edge representing the new face which has all vertices
     * between beginEdge and endEdge
     */
    U32 splitFace(U32 beginEdge, U32 endEdge);


    // traversal functions

    inline U32 eEdge(U32 face, U32 slot) const {
        ASSERTX(face < faces.size());
        ASSERTX(slot < faces[face].count);

        return (face*8) + (slot);
    }

    inline bool eCheck(U32 face, U32 slot) const {
        if (face >= faces.size()) {
            LOG("eCheck failed for (face %u, slot %u): invalid face index (%u faces)",
                face, slot, faces.size());
            return false;
        }
        if (slot >= faces[face].count) {
            LOG("eCheck failed for (face %u, slot %u): invalid slot index (%u slots)",
                face, slot, faces[face].count);
            return false;
        }
        return true;
    }
    inline bool eCheck(U32 edge) const {
        return eCheck(edge/8, edge%8);
    }

    inline U32 ePrev(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        const U8 &slotCount = faces[face].count;
        return (face*8) + ((slot+slotCount-1) % slotCount);
    }
    inline U32 ePrev(U32 edge) const {
        return ePrev(edge/8, edge%8);
    }

    inline U32 eNext(U32 face, U32 slot) const {
        ASSERTX(eCheck(face, slot));
        const U8 &slotCount = faces[face].count;
        return (face*8) + ((slot+1) % slotCount);
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

}

#endif

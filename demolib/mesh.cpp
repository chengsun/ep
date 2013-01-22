#include "mesh.h"
#include "demolib.h"
#include <cmath>
#include <cstring>      // memmove

/* meshCheck checks a mesh for contradictions and constraint violations:
 *
 * for every half-edge e:
 *     e = opposite(opposite(e)) != opposite(e)
 *     start(e) = start(next(opposite(e)))
 * all vertices are at distinct positions
 * all faces are planar and convex with vertices anticlockwise
 */
bool meshCheck(const Mesh *mesh)
{
    for (U32 faceIdx = 0, faceSize = mesh->faces.size(); faceIdx < faceSize; faceIdx++) {
        ASSERTX(mesh->faces[faceIdx].count <= 8,
                "meshCheck failed: too many vertices (%u) on face %u",
                mesh->faces[faceIdx].count, faceIdx);
        for (U32 slot = 0; slot < mesh->faces[faceIdx].count; slot++) {
            U32 edge = mesh->edge(faceIdx, slot);
            U32 oppEdge = mesh->eOpposite(edge);
            ASSERTX(mesh->eCheck(oppEdge),
                    "meshCheck failed: invalid eOpposite(e) for "
                    "(face %u, slot %u)", faceIdx, slot);
            U32 opp2Edge = mesh->eOpposite(oppEdge);
            if (edge != opp2Edge) {
                LOG("meshCheck failed: e != eOpposite(eOpposite(e)) for "
                    "(face %u, slot %u)", faceIdx, slot);
                return false;
            }
            if (edge == oppEdge) {
                LOG("meshCheck failed: e == eOpposite(e) for "
                    "(face %u, slot %u)", faceIdx, slot);
                return false;
            }
            if (mesh->vertIdx(edge) != mesh->vertIdx(mesh->eNext(oppEdge))) {
                LOG("meshCheck failed: vertIdx(e) != vertIdx(eNext(eOpposite(e))) for "
                    "(face %u, slot %u)", faceIdx, slot);
                return false;
            }
        }
    }
    // TODO
    return true;
}

/* meshCheckFlags checks a mesh to ensure that the selected flags are all reset
 */
bool meshCheckFlags(const Mesh *mesh)
{
    for (unsigned i = 0, vertSize = mesh->verts.size(); i < vertSize; i++) {
        if (mesh->verts[i].selected) return false;
    }
    for (unsigned i = 0, faceSize = mesh->faces.size(); i < faceSize; i++) {
        if (mesh->faces[i].selected) return false;
    }
    return true;
}

void meshDebugOut(const Mesh *mesh)
{
    for (unsigned f = 0; f < mesh->faces.size(); f++) {
        for (unsigned s = 0; s < mesh->faces[f].count; s++) {
            U32 opp = mesh->eOpposite(f,s);
            LOG("(%u,%u) is v%u. Opposite: (%u,%u)", f,s, mesh->vertIdx(f,s), opp/8,opp%8);
        }
    }
}

Mesh *createRingMesh(unsigned sides)
{
    ASSERTX(sides > 0, "createRingMesh with 0 verts");
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
    for (unsigned slot = 0; slot < sides; slot++, curRot += incRot) {
        MeshVert &thisVert = mesh->verts[slot];
        thisVert.pos = {cosf(curRot), sinf(curRot), 0.f};
        thisVert.normal = thisVert.pos;

        mesh->faces[0].verts[slot] = slot;
        mesh->faces[1].verts[slot] = (sides-slot)%sides;
        mesh->faces[0].opposite[slot] = mesh->edge(1, sides-slot-1);
        mesh->faces[1].opposite[sides-slot-1] = mesh->edge(0, slot);
    }
    return mesh;
}

void meshExtrudeFace(Mesh *mesh, U32 faceIdx, float length)
{
    unsigned oldSize = mesh->faces.size();
    ASSERTX(faceIdx < oldSize, "meshExtrude invalid faceIdx");

    MeshFace &face = mesh->faces[faceIdx];
    mesh->faces.resize(oldSize + face.count);
    for (U32 slot = 0; slot < face.count; slot++) {
        mesh->verts[face.verts[slot]].pos += length * face.normal;
        if (slot+1 == face.count) continue;
        MeshFace &newFace = mesh->faces[oldSize + slot];
        newFace.count = 4;
        newFace.material = face.material;
        // XXX: is this the right way of calculating the new face normals?
        newFace.normal = Vec3::cross(face.normal,
                                     mesh->verts[face.verts[slot+1]].pos -
                                     mesh->verts[face.verts[slot]].pos);
    }
}

void meshExtrude(Mesh *mesh, U32 startFaceIdx, float length)
{
    unsigned oldSize = mesh->faces.size();
    ASSERTX(startFaceIdx < oldSize, "meshExtrude invalid faceIdx");

    std::vector<U32> edgeList;      // list of boundary edges
    std::vector<U32> faceStack;     // stack for dfs
    faceStack.push_back(startFaceIdx);
    while (!faceStack.empty()) {
        U32 faceIdx = faceStack.back();
        faceStack.pop_back();
        for (U32 slot = 0; slot < mesh->faces[faceIdx].count; slot++) {
            U32 nextFaceIdx = mesh->faces[faceIdx].opposite[slot] / 8;
            if (mesh->faces[nextFaceIdx].selected == 1) {
                // visit this face only if selected and not visited before
                mesh->faces[nextFaceIdx].selected = 2;  // selected but visited before
                faceStack.push_back(nextFaceIdx);
            }
            if (mesh->faces[nextFaceIdx].selected == 0) {
                // this is a boundary edge only if other face not selected
                edgeList.push_back(faceIdx*8 + slot);
            }
        }
    }
    for (unsigned edgeListIdx = 0; edgeListIdx < edgeList.size(); edgeListIdx++) {
        // make a face
        // set the parameters of the new face
    }
}

/* helper function that duplicates the slot before baseEdge
 * v0 ---e--> v1
 * dupVertex(e) creates a new edge e0 and a duplicate slot.
 * v0 --e0--> v0 ---e--> v1
 * the new value of e is returned. e0 is equal to baseEdge after the operation.
 */
static U32 _meshDupVertex(Mesh *mesh, U32 baseEdge)
{
    ASSERTX(mesh->faces[baseEdge/8].count+1u <= MAXVERT,
            "meshDupVertex on face which will have too many vertices");

    MeshFace &face = mesh->faces[baseEdge/8];
    for (unsigned slot = mesh->faces[baseEdge/8].count++; slot-- > baseEdge%8;) {
        const unsigned newSlot = slot+1;
        face.verts[newSlot] = face.verts[slot];
        face.opposite[newSlot] = face.opposite[slot];
        // fix up opposite on adjacent face
        mesh->eOpposite(face.opposite[newSlot]) = mesh->edge(baseEdge/8, newSlot);
    }
    return mesh->eNext(baseEdge);
}

U32 meshSplitVert(Mesh *mesh, U32 beginEdge, U32 endEdge)
{
    U32 oldVertIdx = mesh->vertIdx(beginEdge);

    ASSERTX(oldVertIdx == mesh->vertIdx(endEdge),
            "meshSplitVert on two different vertices");

    // create new vertex (as a copy of old vertex)
    U32 newVertIdx = mesh->verts.size();
    mesh->verts.push_back(mesh->verts[oldVertIdx]);

    // add new vertex to two affected faces
    _meshDupVertex(mesh, beginEdge);
    mesh->eOpposite(beginEdge) = endEdge;
    _meshDupVertex(mesh, endEdge);
    // there is a special case when the edges are the same
    mesh->eOpposite(endEdge) = beginEdge + (beginEdge == endEdge ? 1 : 0);

    ASSERTX(meshCheck(mesh));

    // update the vertex on affected faces
    U32 curEdge = mesh->eNext(beginEdge);
    U32 finEdge = mesh->eOpposite(beginEdge);
    while (true) {
        ASSERTX(mesh->vertIdx(curEdge) == oldVertIdx,
                "meshSplitVert on inconsistent mesh (not all half-edges "
                "originating from same vertex) for (face %u, slot %u) has %u but need %u",
                curEdge/8, curEdge%8, mesh->vertIdx(curEdge), oldVertIdx);
        mesh->vertIdx(curEdge) = newVertIdx;
        if (curEdge == finEdge) break;
        curEdge = mesh->eVertPrev(curEdge);
    }

    // return the new half-edge
    return endEdge;
}

MeshBuf *meshGenBuf(Mesh *mesh)
{
    MeshBuf *buf = new MeshBuf;
    for (U32 faceIdx = 0; faceIdx < mesh->faces.size(); faceIdx++) {
        if (faceIdx) buf->idxBuf.push_back(0xFFFF);
        MeshFace &face = mesh->faces[faceIdx];
        for (U32 slotIdx = 0; slotIdx < face.count; slotIdx++) {
            buf->idxBuf.push_back(face.verts[slotIdx]);
        }
    }
    return buf;
}

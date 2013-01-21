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

U32 meshSplitVert(Mesh *mesh, U32 beginEdge, U32 endEdge)
{
    ASSERTX(mesh->vertIdx(beginEdge) == mesh->vertIdx(endEdge),
            "meshSplitVert on two different vertices");

    ASSERTX(mesh->faces[beginEdge/8].count+1u <= MAXVERT &&
            mesh->faces[endEdge/8].count+1u <= MAXVERT,
            "meshSplitVert on face which will have too many vertices");

    U32 oldVertIdx = mesh->vertIdx(beginEdge);

    LOG("old vertex is %u", oldVertIdx);

    // create new vertex (as a copy of old vertex)
    U32 newVertIdx = mesh->verts.size();
    mesh->verts.push_back(mesh->verts[oldVertIdx]);

    LOG("new vertex is %u", newVertIdx);

    // helper function that duplicates the vertex before baseEdge
    // v0 --e0--> v1
    // addNewVertex(e0)
    // v0 --e0--> v0 --e1--> v1
    static const auto addNewVertex = [&, mesh, newVertIdx](U32 baseEdge) {
        U32 nextEdge = mesh->eNext(baseEdge);
        U32 *verts = &mesh->vertIdx(nextEdge);
        LOG("adding new vertex before (%u,%u)", nextEdge/8, nextEdge%8);
        memmove(verts+1, verts, (mesh->faces[nextEdge/8].count - nextEdge%8) * sizeof(*verts));
        mesh->faces[nextEdge/8].count++;
        *verts = oldVertIdx;
    };

    LOG("begin %u end %u", beginEdge, endEdge);
    // add new vertex to two affected faces
    addNewVertex(mesh->eOpposite(beginEdge));
    if (endEdge != beginEdge) {
        addNewVertex(mesh->eOpposite(endEdge));
    }

    meshDebugOut(mesh);
    // beginEdge and endEdge now refer to the newly created edge

    // set opposite correctly on two affected faces
    mesh->eOpposite(beginEdge) = endEdge;
    mesh->eOpposite(endEdge) = beginEdge;

    // update the vertex on affected faces
    LOG("beginEdge is %u,%u", beginEdge/8, beginEdge%8);
    LOG("endEdge is %u,%u", endEdge/8, endEdge%8);
    U32 curEdge = beginEdge;
    while (curEdge != endEdge) {
        LOG("curEdge is %u,%u", curEdge/8, curEdge%8);
        ASSERTX(mesh->vertIdx(curEdge) == oldVertIdx,
                "meshSplitVert on inconsistent mesh (not all half-edges "
                "originating from same vertex) for (face %u, slot %u) has %u but need %u",
                curEdge/8, curEdge%8, mesh->vertIdx(curEdge), oldVertIdx);
        LOG("setting new vertex at %u,%u", curEdge/8, curEdge%8);
        mesh->vertIdx(curEdge) = newVertIdx;
        curEdge = mesh->eVertNext(curEdge);
    }

    // return the new half-edge
    LOG("returning %u,%u", beginEdge/8, beginEdge%8);
    return beginEdge;
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

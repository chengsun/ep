#include "mesh.h"
#include "demolib.h"
#include <cmath>
#include <cstring>      // memmove

/* Mesh::check checks a mesh for contradictions and constraint violations:
 *
 * for every half-edge e:
 *     e = opposite(opposite(e)) != opposite(e)
 *     start(e) = start(next(opposite(e)))
 * all vertices are at distinct positions
 * all faces are planar and convex with vertices anticlockwise
 */
bool Mesh::check() const
{
    for (U32 faceIdx = 0, faceSize = faces.size(); faceIdx < faceSize; faceIdx++) {
        ASSERTX(faces[faceIdx].count >= 2 &&
                faces[faceIdx].count <= 8,
                "Mesh::check failed: invalid number of slots (%u) on face %u",
                faces[faceIdx].count, faceIdx);
        for (U32 slot = 0; slot < faces[faceIdx].count; slot++) {
            U32 edge = eEdge(faceIdx, slot);
            U32 oppEdge = eOpposite(edge);
            ASSERTX(eCheck(oppEdge),
                    "Mesh::check failed: invalid eOpposite(e) for "
                    "(face %u, slot %u)", faceIdx, slot);
            U32 opp2Edge = eOpposite(oppEdge);
            if (edge != opp2Edge) {
                LOG("Mesh::check failed: e != eOpposite(eOpposite(e)) for "
                    "(face %u, slot %u)", faceIdx, slot);
                return false;
            }
            if (edge == oppEdge) {
                LOG("Mesh::check failed: e == eOpposite(e) for "
                    "(face %u, slot %u)", faceIdx, slot);
                return false;
            }
            if (vertIdx(edge) != vertIdx(eNext(oppEdge))) {
                LOG("Mesh::check failed: vertIdx(e) != vertIdx(eNext(eOpposite(e))) for "
                    "(face %u, slot %u)", faceIdx, slot);
                return false;
            }
        }
    }
    // TODO
    return true;
}

bool Mesh::checkFlags() const
{
    for (unsigned i = 0, vertSize = verts.size(); i < vertSize; i++) {
        if (verts[i].selected) return false;
    }
    for (unsigned i = 0, faceSize = faces.size(); i < faceSize; i++) {
        if (faces[i].selected) return false;
    }
    return true;
}

void Mesh::debugOut() const
{
    for (unsigned f = 0; f < faces.size(); f++) {
        for (unsigned s = 0; s < faces[f].count; s++) {
            U32 opp = eOpposite(f,s);
            LOG("(%u,%u) is v%u. Opposite: (%u,%u)", f,s, vertIdx(f,s), opp/8,opp%8);
        }
    }
}

Mesh *Mesh::createRing(unsigned sides)
{
    ASSERTX(sides >= 1 && sides <= MAXVERT,
            "Mesh::createRing with invalid number of vertices (%u)", sides);

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
        mesh->faces[0].opposite[slot] = mesh->eEdge(1, sides-slot-1);
        mesh->faces[1].opposite[sides-slot-1] = mesh->eEdge(0, slot);
    }

    ASSERTX(mesh->check());

    return mesh;
}

/*
void Mesh::extrudeFace(U32 faceIdx, float length)
{
    unsigned oldSize = faces.size();
    ASSERTX(faceIdx < oldSize, "Mesh::extrude invalid faceIdx");

    MeshFace &face = faces[faceIdx];
    faces.resize(oldSize + face.count);
    for (U32 slot = 0; slot < face.count; slot++) {
        verts[face.verts[slot]].pos += length * face.normal;
        if (slot+1 == face.count) continue;
        MeshFace &newFace = faces[oldSize + slot];
        newFace.count = 4;
        newFace.material = face.material;
        // XXX: is this the right way of calculating the new face normals?
        newFace.normal = Vec3::cross(face.normal,
                                     verts[face.verts[slot+1]].pos -
                                     verts[face.verts[slot]].pos);
    }
}

void Mesh::extrude(U32 startFaceIdx, float length)
{
    unsigned oldSize = faces.size();
    ASSERTX(startFaceIdx < oldSize, "Mesh::extrude invalid faceIdx");

    std::vector<U32> edgeList;      // list of boundary edges
    std::vector<U32> faceStack;     // stack for dfs
    faceStack.push_back(startFaceIdx);
    while (!faceStack.empty()) {
        U32 faceIdx = faceStack.back();
        faceStack.pop_back();
        for (U32 slot = 0; slot < faces[faceIdx].count; slot++) {
            U32 nextFaceIdx = faces[faceIdx].opposite[slot] / 8;
            if (faces[nextFaceIdx].selected == 1) {
                // visit this face only if selected and not visited before
                faces[nextFaceIdx].selected = 2;  // selected but visited before
                faceStack.push_back(nextFaceIdx);
            }
            if (faces[nextFaceIdx].selected == 0) {
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
*/

U32 Mesh::dupVert(U32 baseEdge)
{
    ASSERTX(faces[baseEdge/8].count+1u <= MAXVERT,
            "Mesh::dupVertex on face which will have too many vertices");

    MeshFace &face = faces[baseEdge/8];
    for (unsigned slot = faces[baseEdge/8].count++; slot-- > baseEdge%8;) {
        const unsigned newSlot = slot+1;
        face.verts[newSlot] = face.verts[slot];
        face.opposite[newSlot] = face.opposite[slot];
        // fix up opposite on adjacent face
        eOpposite(face.opposite[newSlot]) = eEdge(baseEdge/8, newSlot);
    }
    return eNext(baseEdge);
}

U32 Mesh::splitVert(U32 beginEdge, U32 endEdge)
{
    ASSERTX(check());

    ASSERTX(vertIdx(beginEdge) == vertIdx(endEdge),
            "Mesh::splitVert on two different vertices");
    U32 oldVertIdx = vertIdx(beginEdge);

    // create new vertex (as a copy of old vertex)
    U32 newVertIdx = verts.size();
    verts.push_back(verts[oldVertIdx]);

    // add new vertex to two affected faces
    dupVert(beginEdge);
    eOpposite(beginEdge) = endEdge;
    dupVert(endEdge);
    // there is a special case when the edges are the same
    eOpposite(endEdge) = beginEdge + (beginEdge == endEdge ? 1 : 0);

    ASSERTX(check());

    // update the vertex on affected faces
    U32 curEdge = eNext(beginEdge);
    U32 finEdge = eOpposite(beginEdge);
    while (true) {
        ASSERTX(vertIdx(curEdge) == oldVertIdx,
                "Mesh::splitVert on inconsistent mesh (not all half-edges "
                "originating from same vertex) for (face %u, slot %u) has %u but need %u",
                curEdge/8, curEdge%8, vertIdx(curEdge), oldVertIdx);
        vertIdx(curEdge) = newVertIdx;
        if (curEdge == finEdge) break;
        curEdge = eVertPrev(curEdge);
    }

    ASSERTX(check());

    // return the new half-edge
    return eOpposite(beginEdge);
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

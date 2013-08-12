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
        if (verts[i].mask) return false;
    }
    for (unsigned i = 0, faceSize = faces.size(); i < faceSize; i++) {
        if (faces[i].mask) return false;
    }
    return true;
}

void Mesh::debugOut() const
{
    for (unsigned f = 0; f < faces.size(); f++) {
        for (unsigned s = 0; s < faces[f].count; s++) {
            U32 opp = eOpposite(f,s);
            const glm::vec3 &pos = verts[vertIdx(f,s)].pos;
            const glm::vec2 &texCoord = verts[vertIdx(f,s)].texCoord;
            LOG("(%u,%u) is v%u. Opposite: (%u,%u) Pos: (%2.3f,%2.3f,%2.3f) Tex: (%2.3f,%2.3f)",
                   f, s, vertIdx(f,s), opp/8,opp%8, (double) pos.x, (double) pos.y, (double) pos.z, (double) texCoord.x, (double) texCoord.y);
        }
    }
}

std::unique_ptr<Mesh> Mesh::createRing(unsigned sides, float phase, float radius)
{
    ASSERTX(sides >= 1 && sides <= MAXVERT,
            "Mesh::createRing with invalid number of vertices (%u)", sides);

    std::unique_ptr<Mesh> mesh(new Mesh);
    mesh->verts.resize(sides);

    mesh->faces.resize(2);
    mesh->faces[0].count = sides;
    mesh->faces[1].count = sides;
    mesh->faces[0].material = 0;
    mesh->faces[1].material = 0;
    mesh->faces[0].normal = glm::vec3(0.f, 0.f,  1.f);
    mesh->faces[1].normal = glm::vec3(0.f, 0.f, -1.f);

    float curRot = 0.f, incRot = 2*PI / sides;
    for (unsigned slot = 0; slot < sides; slot++, curRot += incRot) {
        MeshVert &thisVert = mesh->verts[slot];
        thisVert.pos = glm::vec3(cosf(phase+curRot)*radius,
                                 sinf(phase+curRot)*radius,
                            0.f);
        thisVert.normal = thisVert.pos;
        thisVert.texCoord = (glm::vec2(thisVert.pos) + glm::vec2(1.f,1.f)) / 2.f;

        mesh->faces[0].verts[slot] = slot;
        mesh->faces[1].verts[slot] = (sides-slot)%sides;
        mesh->faces[0].opposite[slot] = mesh->eEdge(1, sides-slot-1);
        mesh->faces[1].opposite[sides-slot-1] = mesh->eEdge(0, slot);
    }

    ASSERTX(mesh->check());

    return mesh;
}

std::unique_ptr<Mesh> Mesh::createGrid(int w, int h)
{
    std::unique_ptr<Mesh> m = Mesh::createRing(4, PI/4.f);

    /* first slice vertically from left to right */
    const glm::vec3 botL = m->verts[m->vertIdx(0,0)].pos,
                    botR = m->verts[m->vertIdx(0,1)].pos,
                    topL = m->verts[m->vertIdx(0,3)].pos,
                    topR = m->verts[m->vertIdx(0,2)].pos;

    for (int x = w-1; x > 0; --x) {
        U32 eBotNew = m->splitVert(m->eEdge(0,0), m->eVertPrev(m->eEdge(0,0)));
        // (0,3) is formerly (0,2)
        U32 eTopNew = m->splitVert(m->eEdge(0,3), m->eVertPrev(m->eEdge(0,3)));
        m->verts[m->vertIdx(eBotNew)].pos = glm::mix(botL, botR, static_cast<float>(x)/w);
        m->verts[m->vertIdx(eTopNew)].pos = glm::mix(topL, topR, static_cast<float>(x)/w);
        m->splitFace(m->eVertNext(eBotNew), m->eVertNext(eTopNew));
        m->splitFace(eBotNew, eTopNew);
    }

    /* then slice horizontally from top to bottom */
    int nFaces = m->faces.size();
    for (int y = 0; y < h; ++y) {
        for (int f = 0; f < /*nFaces*/1; ++f) {
            /* make a split to the left */
            const glm::vec3 rowL = glm::mix(m->verts[m->vertIdx(f,3)].pos,
                                            m->verts[m->vertIdx(f,0)].pos,
                                            static_cast<float>(y)/h);
            U32 eLeftNew = m->splitVert(m->eEdge(f, 3), m->eVertPrev(m->eEdge(f, 3)));
            LOG("(face %u, slot %u)", eLeftNew/8, eLeftNew%8);
            m->verts[m->vertIdx(eLeftNew)].pos = rowL;
            //m->splitFace(eRightNew, eLeftNew);
        }
    }

    return m;
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
        newFace.normal = glm::vec3::cross(face.normal,
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

U32 Mesh::splitFace(U32 beginEdge, U32 endEdge)
{
    ASSERTX(check());

    ASSERTX(beginEdge/8 == endEdge/8, "meshSplitFace on two different faces");
    U32 oldFaceIdx = beginEdge/8;

    ASSERTX(beginEdge != endEdge, "meshSplitFace on same slots");

    // create new face (as a copy of old face)
    U32 newFaceIdx = faces.size();
    faces.push_back(faces[oldFaceIdx]);

    MeshFace &oldFace = faces[oldFaceIdx];

    U32 begin = beginEdge%8, end = endEdge%8;

    unsigned contFaceIdx = newFaceIdx,
             splitFaceIdx = oldFaceIdx;
    if (begin > end) {
        std::swap(begin, end);
        std::swap(contFaceIdx, splitFaceIdx);
    }
    MeshFace &contFace = faces[contFaceIdx],
             &splitFace = faces[splitFaceIdx];

    unsigned faceCount = oldFace.count,
             contFaceCount = end - begin + 1,
             splitFaceCount = begin + faceCount - end + 1;

    contFace.count = 0;
    splitFace.count = 0;

    for (unsigned slot = 0; slot < faceCount; slot++) {
        /* as oldFace.count <= slot, it is always safe to read
         * oldFace.verts[slot] */
        if (begin <= slot && slot <= end) {
            unsigned thisSlot = contFace.count++;
            contFace.verts[thisSlot] = oldFace.verts[slot];
            contFace.opposite[thisSlot] = oldFace.opposite[slot];
            if (thisSlot != contFaceCount-1) {
                eOpposite(contFace.opposite[thisSlot]) = eEdge(contFaceIdx, thisSlot);
            }
        }
        if (slot <= begin || end <= slot) {
            unsigned thisSlot = splitFace.count++;
            splitFace.verts[thisSlot] = oldFace.verts[slot];
            splitFace.opposite[thisSlot] = oldFace.opposite[slot];
            if (thisSlot != begin) {
                eOpposite(splitFace.opposite[thisSlot]) = eEdge(splitFaceIdx, thisSlot);
            }
        }
    }
    ASSERTX(contFace.count == contFaceCount);
    ASSERTX(splitFace.count == splitFaceCount);

    eOpposite(eEdge(contFaceIdx, contFace.count-1)) = eEdge(splitFaceIdx, begin);
    eOpposite(eEdge(splitFaceIdx, begin)) = eEdge(contFaceIdx, contFace.count-1);

    ASSERTX(check());

    return eEdge(newFaceIdx,
                 newFaceIdx == contFaceIdx ? contFace.count-1 : begin);
}


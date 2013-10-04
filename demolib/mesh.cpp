#include "mesh.h"
#include "demolib.h"
#include <cmath>
#include <cstring>      // memmove
#include <map>

/* Mesh::check checks a mesh for contradictions and constraint violations:
 *
 * for every half-edge e:
 *     e = opposite(opposite(e)) != opposite(e)
 *     start(e) = start(next(opposite(e)))
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
            if (!samePosVert(vertIdx(edge), vertIdx(eNext(oppEdge)))) {
                LOG("Mesh::check failed: vert(e) not similar to vert(eNext(eOpposite(e))) for "
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

std::unique_ptr<Mesh> Mesh::createRing(unsigned sides, bool crease, float phase, float radius)
{
    ASSERTX(sides >= 1 && sides <= MAXVERT,
            "Mesh::createRing with invalid number of vertices (%u)", sides);

    std::unique_ptr<Mesh> m(new Mesh);
    m->verts.resize(sides * (crease ? 2 : 1));

    m->faces.resize(2);
    m->faces[0].count = sides;
    m->faces[1].count = sides;
    m->faces[0].material = 0;
    m->faces[1].material = 0;

    float curRot = 0.f, incRot = 2*PI / sides;
    for (unsigned slot = 0; slot < sides; slot++, curRot += incRot) {
        MeshVert &thisVert = m->verts[slot];
        thisVert.pos = glm::vec3(cosf(phase+curRot)*radius,
                                 sinf(phase+curRot)*radius,
                                 0.f);
        thisVert.texCoord = (glm::vec2(thisVert.pos) + glm::vec2(1.f,1.f)) / 2.f;

        if (crease) {
            thisVert.normal = glm::vec3(0.f, 0.f, 1.f);

            MeshVert &otherVert = m->verts[sides + (sides-slot)%sides];
            otherVert.pos = thisVert.pos;
            otherVert.normal = glm::vec3(0.f, 0.f, -1.f);
            otherVert.texCoord = thisVert.texCoord;

            m->faces[0].verts[slot] = slot;
            m->faces[1].verts[slot] = sides + slot;
        } else {
            thisVert.normal = thisVert.pos;

            m->faces[0].verts[slot] = slot;
            m->faces[1].verts[slot] = (sides-slot)%sides;
        }
        m->faces[0].opposite[slot] = m->eEdge(1, sides-slot-1);
        m->faces[1].opposite[sides-slot-1] = m->eEdge(0, slot);
    }

    ASSERTX(m->check());

    return m;
}

std::unique_ptr<Mesh> Mesh::createGrid(int w, int h)
{
    std::unique_ptr<Mesh> m = Mesh::createRing(4, true, 5*PI/4.f, sqrt(2));

    /* first slice vertically from left to right */
    const glm::vec3 botL = m->verts[m->vertIdx(0,0)].pos,
                    botR = m->verts[m->vertIdx(0,1)].pos,
                    topL = m->verts[m->vertIdx(0,3)].pos,
                    topR = m->verts[m->vertIdx(0,2)].pos;

    for (int x = 0; x < w-1; ++x) {
        LOG("%d", x);
        U32 eBotNew = m->splitVert(m->eEdge(x*2,0), m->eVertPrev(m->eEdge(x*2,0)));
        m->verts[m->vertIdx(eBotNew)].pos = glm::mix(botL, botR, static_cast<float>(x+1)/w);
        m->verts[m->vertIdx(m->eVertPrev(eBotNew))].pos = glm::mix(botL, botR, static_cast<float>(x+1)/w);


        // (x*2,3) is formerly (x*2,2)
        U32 eTopNew = m->splitVert(m->eEdge(x*2,3), m->eVertPrev(m->eEdge(x*2,3)));
        m->verts[m->vertIdx(eTopNew)].pos = glm::mix(topL, topR, static_cast<float>(x+1)/w);
        m->verts[m->vertIdx(m->eVertPrev(eTopNew))].pos = glm::mix(topL, topR, static_cast<float>(x+1)/w);

        U32 eBotFlip = m->eVertNext(eBotNew);
        U32 eTopFlip = m->eVertNext(eTopNew);
        LOG("splitting with %d and %d, orig %d", eBotNew, eTopNew, m->eEdge(x*2,0));
        m->splitFace(eBotNew, eTopNew);
        m->splitFace(eTopFlip, eBotFlip);
    LOG("split face");
    m->debugOut();
    LOG("");
    }

    /* then slice horizontally from top to bottom */
    int nFaces = m->faces.size();
    for (int y = 1; y < h; ++y) {
        const int fBegin = (y-1)*nFaces, fEnd = fBegin + nFaces;
        const glm::vec3 rowL = glm::mix(m->verts[m->vertIdx(fBegin,0)].pos,
                                        m->verts[m->vertIdx(fBegin,3)].pos,
                                        static_cast<float>(y)/h),
                        rowR = glm::mix(m->verts[m->vertIdx(fEnd-1,1)].pos,
                                        m->verts[m->vertIdx(fEnd-1,2)].pos,
                                        static_cast<float>(y)/h);
        U32 eRowLeft = m->splitVert(m->eEdge(fBegin, 3), m->eVertPrev(m->eEdge(fBegin, 3)));
        m->verts[m->vertIdx(eRowLeft)].pos = rowL;
        m->verts[m->vertIdx(m->eVertPrev(eRowLeft))].pos = rowL;
        for (int x = 0; x < w; ++x) {
            LOG("%d", x);
            m->debugOut();
            const int fOld = (x*2)+fBegin;
            /* make a split to the left */
            const glm::vec3 thisR = glm::mix(rowL, rowR, static_cast<float>(x+1)/h);
            U32 eRightNew = m->splitVert(m->eEdge(fOld, 2), m->eVertPrev(m->eEdge(fOld, 2)));
            m->verts[m->vertIdx(eRightNew)].pos = thisR;
            m->verts[m->vertIdx(m->eVertPrev(eRightNew))].pos = thisR;
            m->splitFace(eRightNew, m->eEdge(fOld, 5));
            m->splitFace(m->eOpposite(m->eEdge(fOld, 5)), m->eOpposite(eRightNew));
        }
    }

    return m;
}

std::unique_ptr<Mesh> Mesh::createGrid_(int w, int h)
{
    std::unique_ptr<Mesh> m(new Mesh);

    m->verts.resize((w+1)*(h+1));


#if 0
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            m->verts.push_back();
        }
    }

    m->faces.resize(2);
    m->faces[0].count = sides;
    m->faces[1].count = sides;
    m->faces[0].material = 0;
    m->faces[1].material = 0;

    float curRot = 0.f, incRot = 2*PI / sides;
    for (unsigned slot = 0; slot < sides; slot++, curRot += incRot) {
        MeshVert &thisVert = m->verts[slot];
        thisVert.pos = glm::vec3(cosf(phase+curRot)*radius,
                                 sinf(phase+curRot)*radius,
                                 0.f);
        thisVert.texCoord = (glm::vec2(thisVert.pos) + glm::vec2(1.f,1.f)) / 2.f;

        if (crease) {
            thisVert.normal = glm::vec3(0.f, 0.f, 1.f);

            MeshVert &otherVert = m->verts[sides + (sides-slot)%sides];
            otherVert.pos = thisVert.pos;
            otherVert.normal = glm::vec3(0.f, 0.f, -1.f);
            otherVert.texCoord = thisVert.texCoord;

            m->faces[0].verts[slot] = slot;
            m->faces[1].verts[slot] = sides + slot;
        } else {
            thisVert.normal = thisVert.pos;

            m->faces[0].verts[slot] = slot;
            m->faces[1].verts[slot] = (sides-slot)%sides;
        }
        m->faces[0].opposite[slot] = m->eEdge(1, sides-slot-1);
        m->faces[1].opposite[sides-slot-1] = m->eEdge(0, slot);
    }

    ASSERTX(m->check());

    /* first slice vertically from left to right */
    const glm::vec3 botL = m->verts[m->vertIdx(0,0)].pos,
                    botR = m->verts[m->vertIdx(0,1)].pos,
                    topL = m->verts[m->vertIdx(0,3)].pos,
                    topR = m->verts[m->vertIdx(0,2)].pos;

    for (int x = 0; x < w-1; ++x) {
        LOG("%d", x);
        U32 eBotNew = m->splitVert(m->eEdge(x*2,0), m->eVertPrev(m->eEdge(x*2,0)));
        m->verts[m->vertIdx(eBotNew)].pos = glm::mix(botL, botR, static_cast<float>(x+1)/w);
        m->verts[m->vertIdx(m->eVertPrev(eBotNew))].pos = glm::mix(botL, botR, static_cast<float>(x+1)/w);


        // (x*2,3) is formerly (x*2,2)
        U32 eTopNew = m->splitVert(m->eEdge(x*2,3), m->eVertPrev(m->eEdge(x*2,3)));
        m->verts[m->vertIdx(eTopNew)].pos = glm::mix(topL, topR, static_cast<float>(x+1)/w);
        m->verts[m->vertIdx(m->eVertPrev(eTopNew))].pos = glm::mix(topL, topR, static_cast<float>(x+1)/w);

        U32 eBotFlip = m->eVertNext(eBotNew);
        U32 eTopFlip = m->eVertNext(eTopNew);
        LOG("splitting with %d and %d, orig %d", eBotNew, eTopNew, m->eEdge(x*2,0));
        m->splitFace(eBotNew, eTopNew);
        m->splitFace(eTopFlip, eBotFlip);
    LOG("split face");
    m->debugOut();
    LOG("");
    }

    /* then slice horizontally from top to bottom */
    int nFaces = m->faces.size();
    for (int y = 1; y < h; ++y) {
        const int fBegin = (y-1)*nFaces, fEnd = fBegin + nFaces;
        const glm::vec3 rowL = glm::mix(m->verts[m->vertIdx(fBegin,0)].pos,
                                        m->verts[m->vertIdx(fBegin,3)].pos,
                                        static_cast<float>(y)/h),
                        rowR = glm::mix(m->verts[m->vertIdx(fEnd-1,1)].pos,
                                        m->verts[m->vertIdx(fEnd-1,2)].pos,
                                        static_cast<float>(y)/h);
        U32 eRowLeft = m->splitVert(m->eEdge(fBegin, 3), m->eVertPrev(m->eEdge(fBegin, 3)));
        m->verts[m->vertIdx(eRowLeft)].pos = rowL;
        m->verts[m->vertIdx(m->eVertPrev(eRowLeft))].pos = rowL;
        for (int x = 0; x < w; ++x) {
            LOG("%d", x);
            m->debugOut();
            const int fOld = (x*2)+fBegin;
            /* make a split to the left */
            const glm::vec3 thisR = glm::mix(rowL, rowR, static_cast<float>(x+1)/h);
            U32 eRightNew = m->splitVert(m->eEdge(fOld, 2), m->eVertPrev(m->eEdge(fOld, 2)));
            m->verts[m->vertIdx(eRightNew)].pos = thisR;
            m->verts[m->vertIdx(m->eVertPrev(eRightNew))].pos = thisR;
            m->splitFace(eRightNew, m->eEdge(fOld, 5));
            m->splitFace(m->eOpposite(m->eEdge(fOld, 5)), m->eOpposite(eRightNew));
        }
    }

    return m;
#endif
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

/* TODO: weight specifies how much the two split vertices are influenced by
 * the neighbouring vertices */
U32 Mesh::splitVert(U32 beginEdge, U32 endEdge/*, float weight*/)
{
    ASSERTX(check());

    ASSERTX(samePosVert(vertIdx(beginEdge), vertIdx(endEdge)),
            "Mesh::splitVert on two different vertices");
    U32 oldVertIdx = vertIdx(beginEdge);

    U32 newVertIdxBase = verts.size();

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
    std::map<int, int> vertMap;
    while (true) {
        /*
        ASSERTX(vertIdx(curEdge) == oldVertIdx,
                "Mesh::splitVert on inconsistent mesh (not all half-edges "
                "originating from same vertex) for (face %u, slot %u) has %u but need %u",
                curEdge/8, curEdge%8, vertIdx(curEdge), oldVertIdx);
                */
        auto vertMapIns = vertMap.insert(std::make_pair(vertIdx(curEdge),
                                         newVertIdxBase + vertMap.size()));
        vertIdx(curEdge) = vertMapIns.first->second;
        if (curEdge == finEdge) break;
        curEdge = eVertPrev(curEdge);
    }

    // create new vertices (as copies of old vertex)
    verts.insert(verts.end(), vertMap.size(), verts[oldVertIdx]);

    ASSERTX(check());

    // return what used to be beginEdge, but has the new vertex
    return eNext(beginEdge);
}

#if 0
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
    /* contFace contains the continuous slots,
     * splitFace contains the slots that include the discontinuity */
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

#endif




U32 Mesh::splitFace(U32 beginEdge, U32 endEdge)
{
    ASSERTX(check());

    ASSERTX(beginEdge/8 == endEdge/8, "meshSplitFace on two different faces");
    ASSERTX(beginEdge != endEdge, "meshSplitFace on same slots");

    const U32 oldFaceIdx = beginEdge/8,
              newFaceIdx = faces.size();
    // create new face (as a copy of old face)
    faces.push_back(faces[oldFaceIdx]);

    const U32 begin = beginEdge%8, end = endEdge%8;

    MeshFace &oldFace = faces[oldFaceIdx],
             &newFace = faces.back();
    const unsigned origFaceCount = oldFace.count;

    // set new face count
    newFace.count = (origFaceCount + end - begin) % origFaceCount + 1;
    oldFace.count = origFaceCount + 2 - newFace.count;

    // traverse from begin to end and move these into newFace
    for (U32 dstSlot = 0, srcSlot = begin; ;
         ++dstSlot, srcSlot = (srcSlot+1) % origFaceCount) {
        newFace.verts[dstSlot] = oldFace.verts[srcSlot];
        newFace.opposite[dstSlot] = oldFace.opposite[srcSlot];
        if (srcSlot == end) {
            ASSERTX(dstSlot == newFace.count-1u);
            break;
        } else {
            eOpposite(newFace.opposite[dstSlot]) = eEdge(newFaceIdx, dstSlot);
        }
    }
    // fill in the gap in oldFace (if there is one)
    if (end > 0) {
        U32 dstStart = (end > begin ? begin+1 : 0);
        U32 srcEnd = (end > begin ? origFaceCount-1u : begin);
        for (U32 dstSlot = dstStart, srcSlot = end; ;
             ++dstSlot, ++srcSlot) {
            oldFace.verts[dstSlot] = oldFace.verts[srcSlot];
            oldFace.opposite[dstSlot] = oldFace.opposite[srcSlot];
            if (srcSlot != begin) {
                eOpposite(oldFace.opposite[dstSlot]) = eEdge(oldFaceIdx, dstSlot);
            }
            if (srcSlot == srcEnd) {
                ASSERTX(dstSlot == oldFace.count-1u);
                break;
            }
        }
    }

    // fix up opposites on newly created edge
    U32 retEdge = beginEdge - (end > begin ? 0 : end);
    eOpposite(eEdge(newFaceIdx, newFace.count-1)) = retEdge;
    eOpposite(retEdge) = eEdge(newFaceIdx, newFace.count-1);

    return eOpposite(retEdge);
}


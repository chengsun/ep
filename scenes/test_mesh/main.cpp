#include "demo.h"
#include "mesh.h"
#include "text.h"
#include "pngwrite.h"

std::unique_ptr<Mesh> mesh;
ProgramMesh *program;
ProgramTextSDF *pTextSDF;
std::vector<std::unique_ptr<TextureTextSDF> > sdfNums;

void demo_init(unsigned, unsigned)
{
    program = new ProgramMeshDebugVisEdge;
    program->link();
    pTextSDF = new ProgramTextSDF(0);
    pTextSDF->link();

    /*
    mesh = Mesh::createRing(4, PI/4.f);
    mesh->splitVert(mesh->eEdge(0,0), mesh->eVertPrev(0,0));
    mesh->splitVert(mesh->eEdge(0,2), mesh->eVertPrev(0,2));
    mesh->splitVert(mesh->eEdge(0,4), mesh->eVertPrev(0,4));
    mesh->splitVert(mesh->eEdge(0,6), mesh->eVertPrev(0,6));
    mesh->verts[4].pos.x = 0;
    mesh->verts[6].pos.x = 0;
    mesh->verts[5].pos.y = 0;
    mesh->verts[7].pos.y = 0;
    U32 edge = mesh->splitFace(mesh->eEdge(0,1), mesh->eEdge(0,5));
    mesh->splitVert(edge, mesh->eVertPrev(edge));
    mesh->debugOut();
    ASSERTX(mesh->check());
    */

    mesh = Mesh::createGrid(3, 3);
    mesh->debugOut();
    ASSERTX(mesh->check());

    program->updateMeshBuf(*mesh);

    Font font("/usr/share/fonts/TTF/FreeSans.ttf", 300);
    for (int i = 0; i < 10; ++i) {
        sdfNums.push_back(std::unique_ptr<TextureTextSDF>(new TextureTextSDF(64, 64, font.draw(std::to_string(i)), 100)));
        sdfNums[i]->bind(0);
        sdfNums[i]->allocate();
    }
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    static unsigned i = 999;
    static unsigned u = 999;
    if (++u > 100) {
        u = 0;
        mesh->faces[i].mask = 0x00;
        if (++i >= mesh->faces.size()) i = 0;
        mesh->faces[i].mask = 0xFF;
        program->updateMeshBuf(*mesh, 0xFF, 0x00);
    }


    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4Stack ms;
    // camera-to-clip
    ms.push();
    ms.top = glm::perspective(35.0f, 1.f, 0.1f, 100.f);
    // world-to-camera
    ms.push();
    ms *= glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -10.f));

    static float t = 0.f;
    ms.push();
    //ms *= glm::rotate(glm::mat4(), t+=2.f, glm::vec3(0.f,1.f,0.f));
    program->use();
    program->setUniform("uTransform", ms.top);
    program->drawWire();
    program->unuse();

    glDisable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    pTextSDF->use();
//    for (unsigned i = 0; i < mesh->faces.size(); ++i) {
        //glm::vec3 faceNormal;
        glm::vec3 pos;
        const MeshFace &face = mesh->faces[i];
        for (int j = 0; j < face.count; ++j) {
            const MeshVert &vert = mesh->verts[face.verts[j]];
            pos += vert.pos;
            //faceNormal += vert.normal;
        }
        pos /= face.count;
        //faceNormal = glm::normalize(faceNormal);
        //faceNormal = glm::vec3(glm::inverse(glm::transpose(ms.top())) * glm::vec4(faceNormal, 1.f));

        /*
        if (glm::dot(faceNormal, glm::vec3(0.f, 0.f, -1.f)) < 0.f) {
            continue;
        }
        */

        ms.push();
        ms *= glm::translate(glm::mat4(), pos);
        ms *= glm::scale(glm::mat4(), glm::vec3(0.2f, -0.2f, 1.f));
        pTextSDF->setUniform("uTransform", ms.top);
        pTextSDF->setUniform("uTextParms.threshold", 0.5f);
        pTextSDF->setUniform("uTextParms.glow", false);
        sdfNums[i]->bind(0);
        pTextSDF->draw();
        ms.pop();
    //}
    pTextSDF->unuse();
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);

    ms.pop();

    ms.pop();
    ms.pop();
}

void demo_evtMouseButton(uint8_t button, bool state)
{
}

void demo_evtMouseMove(int x, int y)
{
}

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

    mesh = Mesh::createGrid(3, 2);
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4Stack ms;
    // camera-to-clip
    ms.push();
    ms *= glm::perspective(35.0f, 1.f, 0.1f, 100.f);
    // world-to-camera
    ms.push();
    ms *= glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -10.f));

    static float t = 0.f;
    ms.push();
    ms *= glm::rotate(glm::mat4(), t+=2.f, glm::vec3(0.f,1.f,0.f));
    program->use();
    program->setUniform("uTransform", ms.top());
    program->drawWire();
    program->unuse();

    glDisable(GL_DEPTH_TEST);
    pTextSDF->use();
    for (int i = 0; i < mesh->faces.size(); ++i) {
        glm::vec3 pos;
        const MeshFace &face = mesh->faces[i];
        for (int j = 0; j < face.count; ++j) {
            pos += mesh->verts[face.verts[j]].pos;
        }
        pos /= face.count;

        ms.push();
        //ms.curr = glm::translate(glm::mat4(), glm::vec3(ms.curr*glm::vec4(pos, 1.f)));
        ms *= glm::translate(glm::mat4(), pos);
        ms *= glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 1.f));
        pTextSDF->setUniform("uTransform", ms.top());
        pTextSDF->setUniform("uTextParms.threshold", 0.5f);
        pTextSDF->setUniform("uTextParms.glow", false);
        sdfNums[i]->bind(0);
        pTextSDF->draw();
        ms.pop();
    }
    pTextSDF->unuse();
    glEnable(GL_DEPTH_TEST);

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

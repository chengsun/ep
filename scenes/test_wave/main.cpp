#include "demo.h"
#include "wave.h"
#include "program.h"
#include <cmath>

Wave *wave;
TextureWave *waveTex;
ProgramTexturedQuad *program;

int tick = 0;
int stepstep = -1;

void nextstepstep()
{
    tick = 0;
    //stepstep++;
    //if (stepstep > 1) stepstep = 0;
    //wave->reset();
    stepstep=1;
    switch (stepstep) {
    case 0:
        break;
    case 1:
        for (int i = 0; i < 128-16; ++i) {
            wave->Wset(128, i, true);
        }
        for (int i = 128-12; i < 128+12; ++i) {
            wave->Wset(128, i, true);
        }
        for (int i = 128+16; i < 256; ++i) {
            wave->Wset(128, i, true);
        }
        /*
        for (int i = 0; i < 256; ++i) {
            wave->Wset(  0, i, true);
            wave->Wset(255, i, true);
            wave->Wset(i,   0, true);
            wave->Wset(i, 255, true);
        }
        */
        break;
    }
}

void demo_init(unsigned, unsigned)
{
    wave = new Wave(256, 256);
    nextstepstep();
    waveTex = new TextureWave(*wave);
    waveTex->allocate(0);
    program = new ProgramTexturedQuad(0);
    program->link();
    program->debugLink();
}

bool demo_prepareFrame()
{
    switch (stepstep) {
    case 0:
        for (int i = 0; i < 10; ++i) {
            wave->D((tick/50)%256,200) = cosf(tick * 0.05f);
            wave->D(100,(tick/20)%256) = cosf(tick * 0.05f);
            tick++; wave->update();
        }
        if (tick > 2000) nextstepstep();
        return true;
    case 1:
        for (int t = 0; t < 10; ++t) {
            for (int i = 51; i < 205; ++i) {
                wave->D(51,i) = cosf(tick * 0.05f);
            }
            tick++; wave->update();
        }
        if (tick > 2000) nextstepstep();
        return true;
    default:
        return false;
    }
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    waveTex->update(0);

    program->use();
    static float time = 0.f;
    program->setUniform("transform", glm::rotate(glm::mat4(), time++, glm::vec3(1.f, 1.f, 0.f)));
    program->draw();
    program->unuse();
}

void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}

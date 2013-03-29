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
            wave->W(128, i) = 1;
        }
        for (int i = 128-12; i < 128+12; ++i) {
            wave->W(128, i) = 1;
        }
        for (int i = 128+16; i < 256; ++i) {
            wave->W(128, i) = 1;
        }
        /*
        for (int i = 0; i < 256; ++i) {
            wave->W(  0, i) = 1;
            wave->W(255, i) = 1;
            wave->W(i,   0) = 1;
            wave->W(i, 255) = 1;
        }
        */
        break;
    }
}

void demo_init(unsigned, unsigned)
{
    wave = new Wave(256, 256, 0.999f);
    nextstepstep();
    waveTex = new TextureWave(*wave);
    program = new ProgramTexturedQuad;
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
        waveTex->update();
        program->updateTexture(*waveTex);
        if (tick > 2000) nextstepstep();
        return true;
    case 1:
        for (int t = 0; t < 10; ++t) {
            for (int i = 51; i < 205; ++i) {
                wave->D(51,i) = cosf(tick * 0.05f);
            }
            tick++; wave->update();
        }
        waveTex->update();
        program->updateTexture(*waveTex);
        if (tick > 2000) nextstepstep();
        return true;
    default:
        return false;
    }
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    program->draw();
}

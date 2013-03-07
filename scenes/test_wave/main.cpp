#include "demo.h"
#include "wave.h"
#include "program.h"
#include <cmath>

Wave *wave;
TextureWave *waveTex;
ProgramTexturedQuad *program;

void demo_init(unsigned, unsigned)
{
    wave = new Wave(256, 256, 0.999f);
    waveTex = new TextureWave(*wave);
    program = new ProgramTexturedQuad;
}

int tick = 0;

bool demo_prepareFrame()
{
    wave->D(92,128) = (cosf(tick * 0.05f)/2.f +0.5f);
    wave->D(128,92) = (cosf(tick * 0.05f)/2.f +0.5f);
    tick++;
    wave->update();
    waveTex->update();
    program->updateTexture(*waveTex);
    return true;
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    program->draw();
}

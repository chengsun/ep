#ifndef DEMOLIB_WAVE_H
#define DEMOLIB_WAVE_H

#include "util.h"
#include "program.h"
#include <memory>

struct Wave
{
    Wave(unsigned w, unsigned h, float damp);
    ~Wave();

    void reset();
    void update();

    float &D(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return data[x + y*w];
    }
    float &Di(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return datai[x + y*w];
    }
    bool &W(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return dataw[x + y*w];
    }

    // row-major
    float *data, *datai;
    bool *dataw;

    unsigned w, h;
    float damp;
    bool moveDown;
};

struct TextureWave : public Texture2D<uint8_t>
{
    TextureWave(const Wave &wave);
    void update();

    const Wave &wave;
};


#endif

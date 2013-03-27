#ifndef DEMOLIB_WAVE_H
#define DEMOLIB_WAVE_H

#include "util.h"
#include "program.h"
#include <memory>

struct Wave
{
    Wave(unsigned w, unsigned h, float damp);

    void reset();
    void update();

    float &D(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return data.get()[x + y*w];
    }
    float &Di(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return datai.get()[x + y*w];
    }
    bool &W(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return dataw.get()[x + y*w];
    }

    // row-major
    std::shared_ptr<float> data, datai;
    std::shared_ptr<bool> dataw;

    unsigned w, h;
    float damp;
    bool moveDown;
};

struct TextureWave : public Texture2D<float>
{
    TextureWave(const Wave &wave);
    void update();

    const Wave &wave;
};


#endif

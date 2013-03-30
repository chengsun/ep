#ifndef DEMOLIB_WAVE_H
#define DEMOLIB_WAVE_H

#include "util.h"
#include "program.h"
#include <memory>
#include <limits>

struct Wave
{
    Wave(unsigned w, unsigned h, float damp);
    ~Wave();

    void reset();
    void update();

    float &D(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return data[x + y*w].v;
    }
    float &Di(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return data[x + y*w].i;
    }
    bool W(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return std::isnan(Di(x, y));
    }
    void Wset(unsigned x, unsigned y, bool v) {
        ASSERTX(x < w && y < h);
        static_assert(std::numeric_limits<float>::has_signaling_NaN, "No signaling NaN");
        Di(x, y) = v ? std::numeric_limits<float>::signaling_NaN() : 0.0f;
    }

    struct Data {
        float v, i;
    };

    // row-major
    Data *data;

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

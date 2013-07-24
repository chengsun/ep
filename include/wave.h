#ifndef DEMOLIB_WAVE_H
#define DEMOLIB_WAVE_H

#include "util.h"
#include "program.h"
#include <memory>
#include <limits>

struct Wave
{
    Wave(unsigned w, unsigned h);
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
    static const float WALL_VALUE;
    bool W(unsigned x, unsigned y) {
        ASSERTX(x < w && y < h);
        return Di(x, y) == WALL_VALUE;
    }
    void Wset(unsigned x, unsigned y, bool v) {
        ASSERTX(x < w && y < h);
        Di(x, y) = v ? WALL_VALUE : 0.0f;
    }

    struct Data {
        float v, i;
    };

    // row-major
    Data *data;

    unsigned w, h;
    bool moveDown;
};

struct TextureWave : public Texture2D<uint8_t>
{
    TextureWave(const Wave &wave);
    void update(GLuint texUnit);

    const Wave &wave;
};


#endif

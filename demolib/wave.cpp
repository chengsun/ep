#include "program.h"
#include "wave.h"

#include <cstring>
#include <cmath>

Wave::Wave(unsigned _w, unsigned _h, float _damp) :
    w(_w), h(_h), damp(_damp), moveDown(true)
{
    data = std::shared_ptr<float>(new float[w*h],
                                    std::default_delete<float[]>());
    datai = std::shared_ptr<float>(new float[w*h],
                                     std::default_delete<float[]>());
    reset();
}

void Wave::reset()
{
    /*
    memset(data.get(), 0, sizeof(float)*w*h);
    memset(datai.get(), 0, sizeof(float)*w*h);
    */
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < h; ++x) {
            D(x,y) = .5f;
            Di(x,y) = .0f;
        }
    }
}

void Wave::update()
{
    const float tadd = 0.25f;
    const float sinth = sinf(tadd);
    const float scaleo = cosf(tadd);

    unsigned y = 0, yend = h-1, yinc = 1;
    if (!moveDown) {
        std::swap(y, yend); yinc = -1;
    }
    bool moveRight = moveDown;
    for (; ; y += yinc) {
        unsigned x = 0, xend = w-1, xinc = 1;
        if (!moveRight) {
            std::swap(x, xend); xinc = -1;
        }
        for (; ; x += xinc) {
            float n[4];
            if (x == 0)   n[0] = D(w-1, y);
            else          n[0] = D(x-1, y);
            if (x == w-1) n[2] = D(  0, y);
            else          n[2] = D(x+1, y);
            if (y == 0)   n[1] = D(  x, h-1);
            else          n[1] = D(  x, y-1);
            if (y == h-1) n[3] = D(  x,   0);
            else          n[3] = D(  x, y+1);
            float basis = float(n[0] + n[1] + n[2] + n[3]) / 4.f;
            float re = (D(x,y) - basis) * damp,
                  im = Di(x,y) * damp;
            D(x,y) = basis + re*scaleo - im*sinth;
            Di(x,y) = im*scaleo + re*sinth;
            if (x == xend) break;
        }
        moveRight = !moveRight;
        if (y == yend) break;
    }
    moveDown = !moveDown;
}

TextureWave::TextureWave(const Wave &_wave) :
    Texture2D(_wave.w, _wave.h, _wave.data),
    wave(_wave)
{
}

void TextureWave::update()
{
    Texture2D::update();
}

#include "program.h"
#include "wave.h"

#include <cstring>
#include <cmath>

Wave::Wave(unsigned _w, unsigned _h, float _damp) :
    data(std::shared_ptr<float>(new float[_w*_h],
                                std::default_delete<float[]>())),
    datai(std::shared_ptr<float>(new float[_w*_h],
                                 std::default_delete<float[]>())),
    dataw(std::shared_ptr<bool>(new bool[_w*_h],
                                 std::default_delete<bool[]>())),
    w(_w), h(_h),
    damp(_damp), moveDown(true)
{
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
            W(x,y) = 0;
        }
    }
}

void Wave::update()
{
    static const float tadd = 0.1f;
    static const float sinth = sinf(tadd);
    static const float scaleo = cosf(tadd);

    unsigned y = 1, yend = h-2, yinc = 1;
    if (!moveDown) {
        std::swap(y, yend); yinc = -1;
    }
    bool moveRight = moveDown;
    for (; y != yend; y += yinc) {
        unsigned x = 1, xend = w-2, xinc = 1;
        if (!moveRight) {
            std::swap(x, xend); xinc = -1;
        }
        for (; x != xend; x += xinc) {
            if (W(x, y)) continue;

            float basis = 0.f;
            int nx, ny;
            nx = x+1; ny = y;
            if (W(nx, ny)) basis += 0.5f; else basis += D(nx, ny);
            nx = x-1; ny = y;
            if (W(nx, ny)) basis += 0.5f; else basis += D(nx, ny);
            nx = x; ny = y+1;
            if (W(nx, ny)) basis += 0.5f; else basis += D(nx, ny);
            nx = x; ny = y-1;
            if (W(nx, ny)) basis += 0.5f; else basis += D(nx, ny);
            basis /= 4.f;

            static const unsigned edgedamp = 500;
            static const unsigned edgesize = 30;
            unsigned coeff = edgesize - std::min(edgesize, std::min(std::min(x, y), std::min(w-x-1, h-y-1)));

            float re = D(x,y) - basis,
                  im = Di(x,y);

            if (coeff) {
                re = re * (edgedamp-coeff) / edgedamp;
                im = im * (edgedamp-coeff) / edgedamp;
            }

            D(x,y) = basis + re*scaleo - im*sinth;
            Di(x,y) = im*scaleo + re*sinth;
        }
        moveRight = !moveRight;
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

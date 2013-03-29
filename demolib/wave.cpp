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
    static const float theta = 0.1f;
    static const float sintheta = sinf(theta);
    static const float costheta = cosf(theta);

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

#define ADD_BASIS(nx, ny) (W((nx), (ny)) ? 0.5f : D((nx), (ny)))
            float basis = (
                ADD_BASIS(x, y-1) +
                ADD_BASIS(x-1, y) +
                ADD_BASIS(x+1, y) +
                ADD_BASIS(x, y+1)) / 4.f;
#undef ADD_BASIS

            static const unsigned edgedamp = 500;
            static const unsigned edgesize = 30;
            unsigned dist = std::min(std::min(x, y), std::min(w-x-1, h-y-1));

            float re = D(x,y) - basis,
                  im = Di(x,y);

            if (dist < edgesize) {
                re = re * (edgedamp-(edgesize-dist)) / edgedamp;
                im = im * (edgedamp-(edgesize-dist)) / edgedamp;
            }

            D(x,y) = basis + re*costheta - im*sintheta;
            Di(x,y) =        im*costheta + re*sintheta;
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

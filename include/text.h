#ifndef DEMOLIB_TEXT_H
#define DEMOLIB_TEXT_H

#include "program.h"
#include <string>

class Vec2;

class Font
{
    Font(std::string file, int size);
    ~Font();

    Vec2 measure(std::string text);
    SDL_Surface *draw(std::string text);

private:
    TTF_Font *font;
};

class TextureTextSDF : public Texture2D<uint8_t>
{
    TextureTextSDF(SDL_Surface *surf, int scaleDown = 128, int spread = 16);
    ~TextureTextSDF();
    const Font &font;

    SDL_Surface *surface;
};

#endif

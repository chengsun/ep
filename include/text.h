#ifndef DEMOLIB_TEXT_H
#define DEMOLIB_TEXT_H

#include "program.h"
#include <string>

struct SDL_Surface;
typedef struct _TTF_Font TTF_Font;

class Font
{
    Font(std::string file, int size);
    ~Font();

    glm::i32vec2 measure(std::string text);
    SDL_Surface *draw(std::string text);

private:
    TTF_Font *font;
};

class TextureTextSDF : public Texture2D<uint8_t>
{
    TextureTextSDF(SDL_Surface *surf, int scaleDown = 128, int spread = 16);
    ~TextureTextSDF();
};

/*
class ProgramTextSDF : public ProgramMesh
{
};
*/

#endif

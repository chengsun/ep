#ifndef DEMOLIB_TEXT_H
#define DEMOLIB_TEXT_H

#include "program.h"
#include <string>

struct SDL_Surface;
typedef struct _TTF_Font TTF_Font;

struct Font
{
    Font(std::string file, int size);
    ~Font();

    glm::i32vec2 measure(std::string text);
    SDL_Surface *draw(std::string text);

private:
    TTF_Font *font;
};

struct TextureTextSDF : public Texture2D<uint8_t>
{
    TextureTextSDF(SDL_Surface *surf, int scaleDown = 16, int spread = 32);
    TextureTextSDF(int w, int h, SDL_Surface *surf, int spread = 32);
};

struct ProgramTextSDF : public ProgramTexQuadImp
{
    ProgramTextSDF(GLuint texUnit,
            std::initializer_list<std::shared_ptr<Shader> > &&_shaders = {vs, fs});

    static const std::shared_ptr<Shader> fs;
};

#endif

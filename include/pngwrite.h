#ifndef DEMOLIB_PNGWRITE_H
#define DEMOLIB_PNGWRITE_H

#include <SDL.h>
#include "program.h"

bool PNGWrite(SDL_Surface *surf, const char *fileName);
template <typename T>
bool PNGWrite(Texture2D<T> *tex, const char *fileName);

#endif

#include <cstdio>
#include <png.h>
#include <SDL.h>
#include "demolib.h"
#include "pngwrite.h"

static void error_handler(png_structp, png_const_charp msg)
{
    LOG("WARNING: PNGWrite: libpng failed: %s", msg);
}

bool PNGWrite(SDL_Surface *surf, const char *fileName)
{
    bool ret = false;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    FILE *fout = fopen(fileName, "wb");
    if (!fout) {
        LOG("WARNING: PNGWrite: could not open file '%s' for writing", fileName);
        goto cleanup;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, error_handler, NULL);
    if (!png_ptr) {
        LOG("WARNING: PNGWrite: could not create PNG struct");
        goto cleanup;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        LOG("WARNING: PNGWrite: could not create info struct");
        goto cleanup;
    }

    png_init_io(png_ptr, fout);

    png_set_IHDR(png_ptr, info_ptr, surf->w, surf->h,
                 surf->format->BitsPerPixel,
                 surf->format->BitsPerPixel <= 8 ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    {
        png_color colors[256];
        int nColors = surf->format->palette->ncolors;
        for (int i = 0; i < nColors; ++i) {
            colors[i].red   = surf->format->palette->colors[i].r;
            colors[i].green = surf->format->palette->colors[i].g;
            colors[i].blue  = surf->format->palette->colors[i].b;
        }
        png_set_PLTE(png_ptr, info_ptr, colors, nColors);
    }

    png_write_info(png_ptr, info_ptr);

    for (int row = 0; row < surf->h; ++row) {
        png_write_row(png_ptr, static_cast<uint8_t *>(surf->pixels) + row*surf->pitch);
    }

    png_write_end(png_ptr, NULL);

    ret = true;

cleanup:
    if (png_ptr) {
        png_destroy_write_struct(&png_ptr, info_ptr ? &info_ptr : NULL);
    }
    fclose(fout);

    return ret;
}

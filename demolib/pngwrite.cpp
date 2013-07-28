#include <cstdio>
#include <png.h>
#include <SDL.h>
#include <functional>
#include "demolib.h"
#include "pngwrite.h"

static void error_handler(png_structp, png_const_charp msg)
{
    LOG("WARNING: PNGWrite: libpng failed: %s", msg);
}

static bool DoPNGWrite(const char *fileName, uint8_t *data,
                       int w, int h, int stride,
                       std::function<void(png_structp, png_infop)> setMetadata)
{
    LOG("Writing PNG of %dx%d to '%s'", w, h, fileName);
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

    setMetadata(png_ptr, info_ptr);

    png_write_info(png_ptr, info_ptr);

    for (int row = 0; row < h; ++row) {
        png_write_row(png_ptr, data + row*stride);
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

bool PNGWrite(SDL_Surface *surf, const char *fileName)
{
    return DoPNGWrite(fileName, static_cast<uint8_t *>(surf->pixels), surf->w, surf->h, surf->pitch, [&](png_structp png_ptr, png_infop info_ptr) {
        bool pal = surf->format->BitsPerPixel <= 8;
        png_set_IHDR(png_ptr, info_ptr, surf->w, surf->h,
                     surf->format->BitsPerPixel,
                     pal ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGB_ALPHA,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        if (pal) {
            png_color colors[256];
            int nColors = surf->format->palette->ncolors;
            for (int i = 0; i < nColors; ++i) {
                colors[i].red   = surf->format->palette->colors[i].r;
                colors[i].green = surf->format->palette->colors[i].g;
                colors[i].blue  = surf->format->palette->colors[i].b;
            }
            png_set_PLTE(png_ptr, info_ptr, colors, nColors);
        }
    });
}

template <typename T>
struct TextureData {
    static const int colorType;
    static const int bitDepth;
};

template <typename T> const int TextureData<T>::colorType = PNG_COLOR_TYPE_GRAY;
template <typename T> const int TextureData<T>::bitDepth = sizeof(T)*8;
template <> const int TextureData<glm::u8vec4>::colorType = PNG_COLOR_TYPE_RGB_ALPHA;
template <> const int TextureData<glm::u8vec4>::bitDepth = sizeof(int8_t)*8;

template <typename T>
bool PNGWrite(Texture2D<T> *tex, const char *fileName)
{
    int stride = tex->stride > 0 ? tex->stride : tex->w * sizeof(T);
    return DoPNGWrite(fileName, (uint8_t *)tex->data, tex->w, tex->h, stride, [&](png_structp png_ptr, png_infop info_ptr) {
        png_set_IHDR(png_ptr, info_ptr, tex->w, tex->h,
                     TextureData<T>::bitDepth,
                     TextureData<T>::colorType,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    });
}

template bool PNGWrite<uint8_t>(Texture2D<uint8_t> *surf, const char *fileName);
template bool PNGWrite<glm::u8vec4>(Texture2D<glm::u8vec4> *surf, const char *fileName);

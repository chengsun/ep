#include "text.h"
#include "SDL_ttf.h"
#include <string>

Font::Font(std::string file, int size)
{
    font = TTF_OpenFont(file.c_str(), size);
    ASSERTX(font, "Font %s @ %dpt failed to load", file.c_str(), size);
}

Font::~Font()
{
    TTF_CloseFont(font);
}

glm::i32vec2 Font::measure(std::string text)
{
    glm::i32vec2 size;
    if (!TTF_SizeText(font, text.c_str(), &size.x, &size.y)) {
        ASSERTX(font, "Font: could not measure size");
    }
    return size;
}

SDL_Surface *Font::draw(std::string text)
{
    SDL_Surface *ret = TTF_RenderText_Solid(font, text.c_str(), SDL_Color{0,0,0,0});
    ASSERTX(ret, "Font: could not draw text");
    return ret;
}

TextureTextSDF::TextureTextSDF(SDL_Surface *surf, int scale, int spread) :
    Texture2D<uint8_t>()
{
    w = surf->w / scale;
    h = surf->h / scale;
    data = new uint8_t[w*h];
    const uint8_t *odata = static_cast<uint8_t *>(surf->pixels);
    /*
    for (int y = 0; y < surf->h; ++y) {
        for (int x = 0; x < surf->w; ++x) {
            if (odata[y*surf->pitch + x] == 0) {
                continue;
            }
            bool edge = false;
            const int dx[8] = {-1, 0, 1, 1, 1, 0,-1,-1},
                      dy[8] = {-1,-1,-1, 0, 1, 1, 1, 0};
            for (int i = 0; i < 8; ++i) {
                int ny = y + dy[i],
                    nx = x + dx[i];
                if (ny >= 0 && ny < surf->h &&
                    nx >= 0 && nx < surf->w) {
                    if (odata[ny*surf->pitch + nx] == 0) {
                        edge = true;
                        break;
                    }
                }
            }
            if (!edge) {
                continue;
            }
            for (int ty = 0, tyScaled = scale/2;
                 ty < h;
                 ++ty, tyScaled += scale) {
                if (std::abs(tyScaled - y) > spread) {
                    continue;
                }
                for (int tx = 0, txScaled = scale/2;
                     tx < w;
                     ++tx, txScaled += scale) {
                    if (std::abs(txScaled - x) > spread) {
                        continue;
                    }
                    double dist = hypot(tyScaled - y, txScaled - x);
                    uint8_t distVal = dist*256./spread;
                    if (data[ty*w + tx] > distVal) {
                        data[ty*w + tx] = distVal;
                    }
                }
            }
        }
    }
    */
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            data[y*w + x] = x;
        }
    }
    SDL_FreeSurface(surf);
}

TextureTextSDF::TextureTextSDF(int w, int h, SDL_Surface *surf, int spread) :
    Texture2D<uint8_t>(w, h)
{
    data = new uint8_t[w*h];
    uint8_t *odata = static_cast<uint8_t *>(surf->pixels);
    float xscale = (float) surf->w / w,
          yscale = (float) surf->h / h;
    LOG("scales are %fx%f", xscale, yscale);
    LOG("min is %fx%f, max is %fx%f", (0+0.5f) * xscale, (0+0.5f) * yscale,  (w-1+0.5f) * xscale, (h-1+0.5f) * yscale);

    /* twiddle the input surface
     * currently bit 1 (lsb) indicates the inside of the font
     * this loop makes bit 2 indicate the border of the font
     */
    for (int y = 0; y < surf->h; ++y) {
        for (int x = 0; x < surf->w; ++x) {
            if (odata[y*surf->pitch + x] == 0) {
                continue;
            }
            const int dx[8] = {-1, 0, 1, 1, 1, 0,-1,-1},
                      dy[8] = {-1,-1,-1, 0, 1, 1, 1, 0};
            for (int i = 0; i < 8; ++i) {
                int ny = y + dy[i],
                    nx = x + dx[i];
                if (ny >= 0 && ny < surf->h &&
                    nx >= 0 && nx < surf->w) {
                    if (odata[ny*surf->pitch + nx] == 0) {
                        odata[y*surf->pitch + x] |= 0x02;
                        break;
                    }
                }
            }
        }
    }

    /* ty, tx are the texture pixel positions
     * tyScaled, txScaled are the corresponding source pixel positions
     */
    for (int ty = 0; ty < h; ++ty) {
        int tyScaled = (ty+0.5f) * yscale;
        ASSERTX(tyScaled >= 0 && tyScaled < surf->h, "got invalid source y %d", tyScaled);
        int prevrSize = 0;
        for (int tx = 0; tx < w; ++tx) {
            int txScaled = (tx+0.5f) * xscale;
            ASSERTX(txScaled >= 0 && txScaled < surf->w, "got invalid source x %d", txScaled);
            /* optimisation: lower bound is xscale less than to left */
            int minrSize = prevrSize - xscale;
            /* upper bound on how far away the edge is */
            int maxrSize = spread;
            for (int rSize = minrSize; rSize <= maxrSize; ++rSize) {
                int rl = txScaled-rSize, brl = std::max(rl, 0),
                    rr = txScaled+rSize, brr = std::min(rr, surf->w-1),
                    rb = tyScaled-rSize, brb = std::max(rb, 0),
                    rt = tyScaled+rSize, brt = std::min(rt, surf->h-1);
                auto check = [&] (int sx, int sy) {
                    ASSERTX(sx >= 0 && sx < surf->w &&
                            sy >= 0 && sy < surf->h, "got invalid source pixel %d %d", sx, sy);
                    if ((odata[sy*surf->pitch + sx] & 0x02) == 0) {
                        return;
                    }
                    /* now we know our source pixel is an edge */
                    /* this is a new upper bound on how far away the edge is */
                    int newMaxrSize = hypot(sy - tyScaled, sx - txScaled);
                    maxrSize = std::min(maxrSize, newMaxrSize);
                };
                if (rl >= 0)      for (int sx = brl, sy = brb; sy <= brt; ++sy) check(sx, sy);
                if (rr < surf->w) for (int sx = brr, sy = brb; sy <= brt; ++sy) check(sx, sy);
                if (rb >= 0)      for (int sx = brl, sy = brb; sx <= brr; ++sx) check(sx, sy);
                if (rt < surf->h) for (int sx = brl, sy = brt; sx <= brr; ++sx) check(sx, sy);
                data[ty*w + tx] = tx;
            }
            /* now maxrSize stores how far away an edge is */
            /* calculate the texel value */
            int sign = (odata[tyScaled*surf->pitch + txScaled] & 1) ? 1 : -1;
            data[ty*w + tx] = 128 + sign * (maxrSize*127./spread);
            prevrSize = maxrSize;
        }
    }
    SDL_FreeSurface(surf);
}


TextureTextSDF::~TextureTextSDF()
{
    delete[] data;
}

const std::shared_ptr<Shader> ProgramTextSDF::fs =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        in vec2 fTexCoord;
        uniform sampler2D tex;
        uniform float threshold = 0.5f;
        uniform vec4 glowColor = vec4(0.f, 1.f, 0.f, 1.f);
        void main()
        {
            vec4 basec = vec4(1.f, 1.f, 1.f, 1.f);
            float texv = texture(tex, fTexCoord).r;
            basec.a = float(texv >= threshold);
            vec4 glowc = glowColor * smoothstep(threshold*0.3f, threshold*1.0f, texv);
            basec = mix(glowc, basec, basec.a);
            gl_FragColor = basec;
        }
    )", "ProgramTextSDF::fs");

ProgramTextSDF::ProgramTextSDF(GLuint texUnit,
        std::initializer_list<std::shared_ptr<Shader> > &&_shaders) :
    ProgramTexturedQuad(texUnit, std::move(_shaders))
{
}

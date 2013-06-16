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
    for (int y = 0; y < surf->h; ++y) {
        for (int x = 0; x < surf->w; ++x) {
            if (odata[y*surf->w + x] == 0) {
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
                    if (odata[ny*surf->w + nx] == 0) {
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
    SDL_FreeSurface(surf);
}
TextureTextSDF::~TextureTextSDF()
{
    delete[] data;
}

/*
Const std::shared_ptr<Shader> ProgramTextSDF::vs =
    Shader::Inline(GL_VERTEX_SHADER, R"(
        #version 130
        in vec4 position;
        void main()
        {
            gl_Position = position;
        }
    )", "ProgramTextSDF::vs");
const std::shared_ptr<Shader> ProgramTextSDF::fs =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        uniform sampler2D tex;
        uniform ivec2 viewSize;
        void main()
        {
            gl_FragColor = texture(tex, gl_FragCoord.xy/viewSize).rrra;
            //gl_FragColor = vec4((gl_FragCoord.x+50.f)/400.f, 0.f, 0.f, 1.f);
        }
    )", "ProgramTextSDF::fs");

ProgramTextSDF::ProgramTextSDF(GLuint _texUnit) :
    ProgramMesh({vs, fs}),
    texId(0), texSampler(0), texUnit(_texUnit)
{
    updateMeshBuf(*Mesh::createRing(4, PI/4.f, 2.f));

    GLuint texUnif = glGetUniformLocation(id, "tex");
    GLuint viewSizeUnif = glGetUniformLocation(id, "viewSize");
    glUseProgram(id);
    glUniform1i(texUnif, texUnit);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform2i(viewSizeUnif, viewport[2], viewport[3]);

    glUseProgram(0);

    glGenSamplers(1, &texSampler);
    glSamplerParameteri(texSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(texSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void ProgramTextSDF::updateTexture(const Texture2DBase &tex)
{
    texId = tex.id;
}

void ProgramTextSDF::draw() const
{
    glUseProgram(id);
    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glPrimitiveRestartIndexNV(PrimitiveRestartIndex);

    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texId);
    glBindSampler(texUnit, texSampler);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLE_FAN, idxBuf.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindSampler(texUnit, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
    glUseProgram(0);
}
*/

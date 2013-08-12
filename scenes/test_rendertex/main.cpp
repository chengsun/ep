#include "demo.h"
#include "mesh.h"
#include "text.h"
#include "pngwrite.h"
#include <SDL.h>
#include <unistd.h>

std::unique_ptr<Mesh> mesh;
ProgramTextSDF *program;
Font *font;
TextureTextSDF *textTex;
Texture2D<glm::u8vec4> *renderTex;
FBO *renderTexFBO;

ProgramTexturedQuad *progMain;

void demo_init(unsigned, unsigned)
{
    const char *str = "Hello world";
    renderTexFBO = new FBO(128, 128, 0x0);
    renderTex = new Texture2D<glm::u8vec4>(128, 128);
    renderTex->bind(0);
    renderTex->allocate();

    program = new ProgramTextSDF(0/*, {ProgramTexturedQuad::vs, ProgramTexturedQuad::fs}*/);
    program->link();
    progMain = new ProgramTexturedQuad(0);
    progMain->link();

    font = new Font("/usr/share/fonts/TTF/FreeSans.ttf", 600);
    auto size = font->measure(str);
    LOG("using size %dx%d", size.x, size.y);
    SDL_Surface *surf = font->draw(str);
    //PNGWrite(surf, "test.png");
    textTex = new TextureTextSDF(128, 128, surf, 200);
    surf = NULL;
    LOG("got size %dx%d", textTex->w, textTex->h);
    textTex->bind(0);
    textTex->allocate();
}

bool demo_prepareFrame()
{
    static int c = 0;
    if (++c == 200) {
        PNGWrite(renderTex, "test.png");
        return false;
    }
    return true;
}

void demo_drawFrame()
{
    static float t = 0.f;

    renderTexFBO->bindTexture(*renderTex, 0);
    GLContext::get().bindFBO(*renderTexFBO);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    textTex->bind(0);

    program->use();
    program->setUniform("transform", glm::mat4());
    program->setUniform("threshold", 0.5f/*clamp((float) fabs(fmod(t/100.f,1.40f)-0.70f)+0.30f, 0.5f, 1.0f)*/);
    program->setUniform("glowColor", glm::vec4(sin(t/100.f)/2+0.5, sin(t/100.f + 2*PI/3)/2+0.5, sin(t/100.f + 4*PI/3)/2+0.5, 1.f));
    t += 1.f;
    program->draw();
    program->unuse();

    GLContext::get().unbindFBO();

    renderTex->bind(0);
    renderTex->updateLocal();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    progMain->use();
    renderTex->bind(0);
    progMain->setUniform("transform", glm::scale(glm::mat4(), glm::vec3(0.5f,-0.2f,1.0f)));
    progMain->draw();
    progMain->unuse();
}

/*
void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}
*/

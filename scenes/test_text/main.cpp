#include "demo.h"
#include "mesh.h"
#include "text.h"
#include "pngwrite.h"
#include <SDL.h>

std::unique_ptr<Mesh> mesh;
ProgramTextSDF *program;
Font *font;
TextureTextSDF *textTex;

void demo_init(unsigned, unsigned)
{
    const char *str = "Hello world";
    program = new ProgramTextSDF(0/*, {ProgramTexturedQuad::vs, ProgramTexturedQuad::fs}*/);
    program->link();
    font = new Font("/usr/share/fonts/TTF/FreeSans.ttf", 600);
    auto size = font->measure(str);
    LOG("using size %dx%d", size.x, size.y);
    SDL_Surface *surf = font->draw(str);
    //PNGWrite(surf, "test.png");
    textTex = new TextureTextSDF(128, 128, surf, 200);
    surf = NULL;
    LOG("got size %dx%d", textTex->w, textTex->h);
    textTex->allocate(0);
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    static float t = 0.f;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    textTex->bind(0);

    program->use();
    glm::mat4 I,
              A = glm::rotate(I, t*3.f, glm::vec3(1.f, 0.f, 0.f)),
              B = glm::rotate(A, t*1.1f, glm::vec3(0.f, 1.f, 0.f)),
              C = glm::scale(B, glm::vec3(1.0f,-0.2f,1.f)),
              D = glm::translate(C, glm::vec3(sin(t/30.f)*0.6f, sin(t/70.f)*0.3f, 0.f));
    program->setUniform("transform", D);
    program->setUniform("threshold", clamp((float) fabs(fmod(t/100.f,1.40f)-0.70f)+0.30f, 0.5f, 1.0f));
    program->setUniform("glowColor", glm::vec4(sin(t/100.f)/2+0.5, sin(t/100.f + 2*PI/3)/2+0.5, sin(t/100.f + 4*PI/3)/2+0.5, 1.f));
    t += 1.f;
    program->draw();
    program->unuse();
}

void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}

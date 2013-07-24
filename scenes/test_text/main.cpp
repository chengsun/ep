#include "demo.h"
#include "mesh.h"
#include "text.h"

std::unique_ptr<Mesh> mesh;
ProgramTexturedQuad *program;
Font *font;
TextureTextSDF *textTex;

void demo_init(unsigned, unsigned)
{
    const char *str = "Hello world";
    program = new ProgramTexturedQuad(0);
    program->link();
    font = new Font("/usr/share/fonts/TTF/FreeSans.ttf", 1200);
    auto size = font->measure(str);
    LOG("using size %dx%d", size.x, size.y);
    textTex = new TextureTextSDF(font->draw(str));
    LOG("got size %dx%d", textTex->w, textTex->h);
    textTex->allocate(0);
}

bool demo_prepareFrame()
{
    return true;
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    textTex->bind(0);

    program->use();
    program->setUniform("transform", glm::mat4()/*glm::scale(glm::mat4(), glm::vec3(1.0f,0.2f,1.f))*/);
    program->draw();
    program->unuse();
}

void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}

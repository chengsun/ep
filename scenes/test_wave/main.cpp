#include "demo.h"
#include "wave.h"
#include "program.h"
#include "pngwrite.h"
#include "text.h"
#include <cmath>
#include <array>

const std::shared_ptr<Shader> wavefs =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        in vec2 vTexCoord;
        uniform mat4 uTransform;
        uniform sampler2D uTex;
        const float delta = 0.005f;
        uniform vec3 lightray = vec3(1.f, 1.f, 1.f);
        uniform int shadeStyle = 0;
        const float M_PI = 3.14159265358979f;

        vec3 getSurf(vec2 pos) {
            return vec3(uTransform*vec4(pos, pow(texture(uTex, pos).r, 1.0f), 1.f));
        }

        void main()
        {
            vec3 normal = normalize(cross(
                    getSurf(vTexCoord+vec2(delta, 0.f))-getSurf(vTexCoord+vec2(-delta, 0.f)),
                    getSurf(vTexCoord+vec2(0.f, delta))-getSurf(vTexCoord+vec2(0.f, -delta))));
            vec3 lightdir = normalize(lightray);
            //float angle = clamp(acos(dot(normal, lightdir)), 0.f, M_PI);
            float angle = clamp(dot(normal, lightdir), 0.f, 1);
            float value = clamp(angle/M_PI, 0.f, 1.f);
            vec3 halfang = normalize(lightdir + vec3(0.f, 0.f, -1.f));
            float specval = clamp(dot(normal, halfang), 0.f, 1.f);
            specval = angle != 0.0f ? specval : 0.0f;

            vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * 0.1f;
            vec3 specular = 5.0f*pow(specval, 5.0f) * vec3(0.78f, 0.90f, 1.0f);
            vec3 diffuse = value * vec3(0.38f, 0.45f, 0.9f);

            switch (shadeStyle) {
            case 0:
                gl_FragColor = vec4(ambient + specular + diffuse, 1.f);
                break;
            case 1:
                gl_FragColor = vec4(normal*2, 1.f);
                break;
            case 2:
                gl_FragColor = vec4(specular, 1.f);
                break;
            case 3:
                gl_FragColor = vec4(diffuse, 1.f);
                break;
            }
            //gl_FragColor = vec4(normal*100, 1.f);
        }
    )", "custom wavefs");

Wave *wave;
TextureWave *waveTex;
ProgramTexQuadImp *program;

int tick = 0;
int stepstep = -1;
int shadeStyle = 0;

Font *font;
const char *texts[] = {
    "Moving waves",
    "Double slit experiment",
    "Water drops",
    "Blinn-Phong shading",
    "Normals",
    "Specular",
    "Diffuse"
};
static const int texts_shadeOff = 3;
static const int nShadeStyles = 4;
static constexpr int ntexts = sizeof(texts) / sizeof(texts[0]);
TextureTextSDF *textTexs[ntexts];
ProgramTextSDF *programText;

void nextstepstep()
{
    tick = 0;
    stepstep++;
    if (stepstep > 2) stepstep = 0;
    wave->reset();
    switch (stepstep) {
    case 0:
        break;
    case 1:
        for (int i = 0; i < 128-16; ++i) {
            wave->Wset(128, i, true);
        }
        for (int i = 128-12; i < 128+12; ++i) {
            wave->Wset(128, i, true);
        }
        for (int i = 128+16; i < 256; ++i) {
            wave->Wset(128, i, true);
        }
        break;
    case 2:
        for (int i = 30; i < 226; ++i) {
            wave->Wset( 30, i, true);
            wave->Wset(225, i, true);
            wave->Wset(i,  30, true);
            wave->Wset(i, 225, true);
        }
        break;
    }
}

void demo_init(unsigned, unsigned)
{
    wave = new Wave(256, 256);
    nextstepstep();
    waveTex = new TextureWave(*wave);
    waveTex->bind(0);
    waveTex->allocate();
    program = new ProgramTexQuadImp(0, {ProgramTexQuadImp::vs, wavefs});
    program->link();


    programText = new ProgramTextSDF(1/*, {ProgramTexturedQuad::vs, ProgramTexturedQuad::fs}*/);
    programText->link();
    font = new Font("/usr/share/fonts/TTF/FreeSans.ttf", 500);
    for (int i = 0; i < ntexts; ++i) {
        auto size = font->measure(texts[i]);
        LOG("using size %dx%d", size.x, size.y);
        SDL_Surface *surf = font->draw(texts[i]);
        textTexs[i] = new TextureTextSDF(256, 64, surf, 90);
        surf = NULL;
        textTexs[i]->bind(0);
        textTexs[i]->allocate();
    }
}

bool demo_prepareFrame()
{

    /*
    if (++c == 200) {
        PNGWrite(waveTex, "test.png");
        return false;
    }
    */
    switch (stepstep) {
    case 0:
        for (int i = 0; i < 10; ++i) {
            wave->D(30+(tick/50)%196,200) = cosf(tick * 0.05f);
            wave->D(100,30+(tick/20)%196) = cosf(tick * 0.05f);
            tick++; wave->update();
        }
        //if (tick > 5000) nextstepstep();
        return true;
    case 1:
        for (int t = 0; t < 10; ++t) {
            for (int i = 51; i < 205; ++i) {
                wave->D(51,i) = cosf(tick * 0.05f);
            }
            tick++; wave->update();
        }
        //if (tick > 5000) nextstepstep();
        return true;
    case 2:
    {
        int iters = 6;
        /*if (tick >= 200 && tick <= 600) {
            iters = 2;
        }*/
        for (int t = 0; t < iters; ++t) {
            if (tick % 500 <= 30) {
                wave->D(51,130) = cosf((tick%500)*PI/60.f);
            }
            if ((tick+250) % 500 <= 30) {
                wave->D(141,170) = cosf(((tick+250)%500)*PI/60.f);
            }
            tick++; wave->update();
        }
        //if (tick > 5000) nextstepstep();
        return true;
    }
    default:
        return false;
    }
}

void demo_drawFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    waveTex->bind(0);
    waveTex->update();

    program->use();
    static float time = 0.f;
    //program->setUniform("uTransform", glm::rotate(glm::mat4(), time+=0.1f, glm::vec3(0.1f, 0.3f, 1.f)));
    program->setUniform("uTransform", glm::mat4());
    program->setUniform("uTexScale", 0.75f);
    program->setUniform("shadeStyle", shadeStyle);
    program->setUniform("lightray", glm::vec3(cosf(time/100)*2.f-1.f, sinf(time/100)*2.f-1.f, 1.f));
    program->draw();
    program->unuse();


    programText->use();
    programText->setUniform("uTextParms.threshold", 0.5f);
    programText->setUniform("uTextParms.glowColor", glm::vec4(0.f, 0.f, 0.f, 1.f));
    programText->setUniform("uTextParms.glow", true);

    programText->setUniform("uTransform",
            glm::scale(
                glm::translate(
                    glm::mat4(),
                    glm::vec3(0.4f, -0.6f, 0.0f)
                ),
                glm::vec3(0.08f*strlen(texts[stepstep]), -0.2f, 1.f)*0.3f
            ));
    textTexs[stepstep]->bind(1);
    programText->draw();


    programText->setUniform("uTransform",
            glm::scale(
                glm::translate(
                    glm::mat4(),
                    glm::vec3(0.4f, -0.8f, 0.0f)
                ),
                glm::vec3(0.08f*strlen(texts[shadeStyle + texts_shadeOff]), -0.2f, 1.f)*0.3f
            ));
    textTexs[shadeStyle + texts_shadeOff]->bind(1);
    programText->draw();

    programText->unuse();

    time += 1.f;
}

void demo_evtMouseButton(uint8_t button, bool state)
{
    if (state == false) {
        switch (button) {
        case 1:
            nextstepstep();
            break;
        case 3:
            shadeStyle = (shadeStyle+1) % nShadeStyles;
            break;
        }
    }
}

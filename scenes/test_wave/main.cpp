#include "demo.h"
#include "wave.h"
#include "program.h"
#include "pngwrite.h"
#include <cmath>

const std::shared_ptr<Shader> wavefs =
    Shader::Inline(GL_FRAGMENT_SHADER, R"(
        #version 130
        in vec2 vTexCoord;
        uniform mat4 uTransform;
        uniform sampler2D uTex;
        const float delta = 0.005f;
        uniform vec3 lightray = vec3(1.f, 1.f, 1.f);
        const float M_PI = 3.14159265358979f;

        vec3 getSurf(vec2 pos) {
            return vec3(uTransform*vec4(pos, pow(texture(uTex, pos).r, 0.1f), 1.f));
        }

        void main()
        {
            /*
            vec3 ov = getSurf(vTexCoord);
            vec3 xv = getSurf(vTexCoord+vec2(delta, 0.f));
            vec3 yv = getSurf(vTexCoord+vec2(0.f, delta));
            vec3 normal = cross(xv-ov, yv-ov);
            */
            vec3 normal = normalize(cross(
                    getSurf(vTexCoord+vec2(delta, 0.f))-getSurf(vTexCoord+vec2(-delta, 0.f)),
                    getSurf(vTexCoord+vec2(0.f, delta))-getSurf(vTexCoord+vec2(0.f, -delta))));
            float angle = clamp(acos(dot(normal, lightray) / (length(normal)*length(lightray))), 0.f, M_PI);
            float value = clamp(angle/M_PI, 0.f, 1.f);
            vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
            vec3 specular = 0.1f/(angle+.05f) * vec3(0.78f, 0.90f, 1.0f);
            vec3 diffuse = value * vec3(0.38f, 0.45f, 0.9f);
            gl_FragColor = vec4(ambient + specular + diffuse, 1.f);
            //gl_FragColor = vec4(specular, 1.f);
            //gl_FragColor = vec4(normal*100, 1.f);
        }
    )", "custom wavefs");

Wave *wave;
TextureWave *waveTex;
ProgramTexQuadImp *program;

int tick = 0;
int stepstep = -1;

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
}

bool demo_prepareFrame()
{

    static int c = 0;
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
        if (tick > 5000) nextstepstep();
        return true;
    case 1:
        for (int t = 0; t < 10; ++t) {
            for (int i = 51; i < 205; ++i) {
                wave->D(51,i) = cosf(tick * 0.05f);
            }
            tick++; wave->update();
        }
        if (tick > 5000) nextstepstep();
        return true;
    case 2:
        for (int t = 0; t < 10; ++t) {
            if (tick % 500 <= 30) {
                wave->D(51,130) = cosf((tick%500)*PI/60.f);
            }
            if ((tick+250) % 500 <= 30) {
                wave->D(141,170) = cosf(((tick+250)%500)*PI/60.f);
            }
            tick++; wave->update();
        }
        if (tick > 5000) nextstepstep();
        return true;
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
    program->setUniform("lightray", glm::vec3(cos(time/100)*2.f-1.f, sin(time/100)*2.f-1.f, 1.f));
    time += 1.f;
    program->draw();
    program->unuse();
}

void demo_evtMouseButton(uint8_t button, bool state)
{
    if (state == false) {
        nextstepstep();
    }
}

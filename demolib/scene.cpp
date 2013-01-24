#include "scene.h"

Scene *createScene()
{
    // TODO
    return NULL;
}

void sceneRender(const Scene *scene)
{
    // TODO
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    //meshBuf.draw();
    //glDrawElements(GL_TRIANGLE_FAN, );
}

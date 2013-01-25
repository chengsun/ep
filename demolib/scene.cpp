#include "scene.h"

Scene::Scene()
{
    // TODO
}

void Scene::Render() const
{
    // TODO
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    //meshBuf.draw();
}

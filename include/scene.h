#ifndef DEMOLIB_SCENE_H
#define DEMOLIB_SCENE_H

#include "util.h"
#include "mesh.h"
#include "program.h"

struct SceneNode
{
    Mesh *mesh;
    Program *program;

    glm::vec3 *pos;
    glm::quat *orientation;
    float scale;

    TextureBase *texture;
};

struct Scene
{
    Scene();

    void Render() const;
};

#endif

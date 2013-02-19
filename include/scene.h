#ifndef DEMOLIB_SCENE_H
#define DEMOLIB_SCENE_H

#include "util.h"
#include "mesh.h"
#include "program.h"
#include "texture.h"

struct SceneNode
{
    Mesh *mesh;
    Program *program;

    Vec3 *pos;
    Quat *orientation;
    float scale;

    Texture *texture;
};

struct Scene
{
    Scene();

    void Render() const;
};

#endif

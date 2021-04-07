#ifndef H_OBJECT
#define H_OBJECT

#include <memory>

#include "graphics/mesh.h"
#include "graphics/texture.h"

class Object {
public:
    Object();

    void Draw();

private:
    Mesh* mesh;
    Texture* texture;
};

#endif
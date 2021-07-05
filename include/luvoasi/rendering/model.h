#ifndef H_LUVOASI_MODEL
#define H_LUVOASI_MODEL

#include "luvoasi/core/base.h"

#include "luvoasi/graphics/texture.h"
#include "luvoasi/graphics/vertex_array.h"

namespace Luvoasi {

/**
 * @brief 
 * 
 */
class Model {
public:
    Model();

private:
    VertexArray* va;
    Texture* texture;
};

}

#endif
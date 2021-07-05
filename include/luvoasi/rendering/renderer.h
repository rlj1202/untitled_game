#ifndef H_LUVOASI_RENDERER
#define H_LUVOASI_RENDERER

#include "luvoasi/core/base.h"
#include "luvoasi/graphics/vertex_array.h"

namespace Luvoasi {

/**
 * @brief 
 * @details
 * 
 */
class Renderer {
public:
    void BeginScene();
    void Submit(const VertexArray& va);
    void EndScene();
};

}

#endif
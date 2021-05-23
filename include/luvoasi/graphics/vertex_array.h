#ifndef H_LUVOASI_VERTEX_ARRAY
#define H_LUVOASI_VERTEX_ARRAY

#include "luvoasi/core/base.h"
#include "luvoasi/graphics/buffer.h"

namespace Luvoasi {

class VertexArray {
public:
    virtual ~VertexArray() = default;

    virtual uint32_t GetId() const = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
};

}

#endif
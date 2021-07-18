#ifndef H_LUVOASI_VERTEX_ARRAY
#define H_LUVOASI_VERTEX_ARRAY

#define LUVOASI_DEBUG_ENABLE
#include "luvoasi/core/base.h"
#include "luvoasi/graphics/buffer.h"

namespace Luvoasi {

class VertexArray {
public:
    virtual ~VertexArray() = default;

    virtual uint32_t GetId() const = 0;

    virtual void AttachArrayBuffer(std::unique_ptr<ArrayBuffer> buffer, const BufferLayout &layout) = 0;
    virtual void AttachIndexBuffer(std::unique_ptr<IndexBuffer> buffer) = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void Draw() const = 0;

    static std::unique_ptr<VertexArray> CreateVertexArray();
};

}

#endif
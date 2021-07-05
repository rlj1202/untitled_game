#ifndef H_LUVOASI_VERTEX_ARRAY_OPENGL
#define H_LUVOASI_VERTEX_ARRAY_OPENGL

#include "luvoasi/graphics/vertex_array.h"

namespace Luvoasi {

class OpenGLVertexArray : public VertexArray {
public:
    OpenGLVertexArray();
    OpenGLVertexArray(const OpenGLVertexArray& o) = delete;
    OpenGLVertexArray(OpenGLVertexArray&& o);
    virtual ~OpenGLVertexArray();

    OpenGLVertexArray& operator=(const OpenGLVertexArray& o) = delete;
    OpenGLVertexArray& operator=(OpenGLVertexArray&& o) noexcept;

    uint32_t GetId() const override;

    void AttachArrayBuffer(std::unique_ptr<ArrayBuffer> buffer, const BufferLayout &layout) override;
    void AttachIndexBuffer(std::unique_ptr<IndexBuffer> buffer) override;

    void Bind() const override;
    void Unbind() const override;

    void Draw() const override;

private:
    uint32_t m_id;

    std::vector<std::unique_ptr<ArrayBuffer>> m_buffers;
    std::unique_ptr<IndexBuffer> m_index_buffer;
};

}

#endif
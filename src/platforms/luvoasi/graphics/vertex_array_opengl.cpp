#include "platforms/luvoasi/graphics/vertex_array_opengl.h"

#ifdef EMSCRIPTEN
#include "GLES3/gl3.h"
#else
#include "glad/glad.h"
#endif

#include "platforms/luvoasi/graphics/shader_opengl.h"

namespace Luvoasi {

OpenGLVertexArray::OpenGLVertexArray() {
    glGenVertexArrays(1, &m_id);

    glBindVertexArray(m_id);
}

OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexArray&& o) {
    m_id = o.m_id;
    o.m_id = 0;

    m_buffers = std::move(o.m_buffers);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    if (m_id) {
        glDeleteVertexArrays(1, &m_id);
        m_id = 0;
    }
}

OpenGLVertexArray& OpenGLVertexArray::operator=(OpenGLVertexArray&& o) noexcept {
    if (this == &o) return *this;

    if (m_id) {
        glDeleteVertexArrays(1, &m_id);
    }
    m_id = o.m_id;
    o.m_id = 0;

    m_buffers = std::move(o.m_buffers);

    return *this;
}

uint32_t OpenGLVertexArray::GetId() const {
    return m_id;
}

void OpenGLVertexArray::AttachArrayBuffer(std::unique_ptr<ArrayBuffer> buffer, const BufferLayout &layout) {
    Bind();
    buffer->Bind();

    uint32_t stride = 0;
    for (const BufferElement element : layout.GetElements()) {
        stride += DataType::GetBytes(element.internal_type) * element.size;
    }

    uint32_t offset = 0;
    for (const BufferElement element : layout.GetElements()) {
        glVertexAttribPointer(
            element.index, element.size,
            GetOpenGLDataType(element.internal_type),
            element.normalize,
            stride,
            (void*) offset
        );
        glEnableVertexAttribArray(element.index);

        offset += DataType::GetBytes(element.internal_type) * element.size;
    }

    Unbind();

    m_buffers.push_back(std::move(buffer));
}

void OpenGLVertexArray::AttachIndexBuffer(std::unique_ptr<IndexBuffer> buffer) {
    Bind();
    buffer->Bind();

    Unbind();

    m_index_buffer = std::move(buffer);
}

void OpenGLVertexArray::Bind() const {
    glBindVertexArray(m_id);
}

void OpenGLVertexArray::Unbind() const {
    glBindVertexArray(0);
}

void OpenGLVertexArray::Draw() const {
    Bind();
    if (m_index_buffer) {
        // TODO:
        int cnt_instances = 1;
        glDrawElementsInstanced(
            GL_TRIANGLES, m_index_buffer->GetLength(), GL_UNSIGNED_INT,
            nullptr, cnt_instances);
    } else {
        // TODO: 
        LUVOASI_DEBUG_STDOUT("ERROR: DrawArrays\n");
        int cnt_vertices = 1;
        glDrawArrays(GL_TRIANGLES, 0, cnt_vertices);
    }
}

std::unique_ptr<VertexArray> VertexArray::CreateVertexArray() {
    return std::move(std::make_unique<OpenGLVertexArray>());
}

}

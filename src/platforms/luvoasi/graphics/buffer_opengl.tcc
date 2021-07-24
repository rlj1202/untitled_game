#include "platforms/luvoasi/graphics/buffer_opengl.h"

#include <type_traits>

#ifdef EMSCRIPTEN
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

namespace Luvoasi {

template<BufferType T, typename U>
OpenGLBuffer<T, U>::OpenGLBuffer(GLenum usage)
    : OpenGLBuffer(usage, nullptr, 0) {

}

template<BufferType T, typename U>
OpenGLBuffer<T, U>::OpenGLBuffer(
    GLenum type, GLenum usage, const void* data, uint32_t size)
    : m_type(type), m_usage(usage), m_length(size) {

    glGenBuffers(1, &m_id);
    Bind();
    glBufferData(m_type, size * sizeof(U), data, m_usage);
}

template<BufferType T, typename U>
OpenGLBuffer<T, U>::OpenGLBuffer(OpenGLBuffer<T, U>&& o) {
    m_id = o.m_id;
    o.m_id = 0;

    m_type = o.m_type;
    m_usage = o.m_usage;
}

template<BufferType T, typename U>
OpenGLBuffer<T, U>::~OpenGLBuffer() {
    if (m_id) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }
}

template<BufferType T, typename U>
OpenGLBuffer<T, U>& OpenGLBuffer<T, U>::operator=(OpenGLBuffer<T, U>&& o) noexcept {
    if (this == &o) return *this;

    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }
    m_id = o.m_id;
    o.m_id = 0;

    m_type = o.m_type;
    m_usage = o.m_usage;

    return *this;
}

template<BufferType T, typename U>
uint32_t OpenGLBuffer<T, U>::GetId() const {
    return m_id;
}

template<BufferType T, typename U>
void OpenGLBuffer<T, U>::SetData(const U* data, uint32_t size) {
    Bind();
    glBufferSubData(m_type, 0, size * sizeof(U), data);
}

template<BufferType T, typename U>
uint32_t OpenGLBuffer<T, U>::GetLength() const {
    return m_length;
}

template<BufferType T, typename U>
void OpenGLBuffer<T, U>::Bind() const {
    glBindBuffer(m_type, m_id);
}

template<BufferType T, typename U>
void OpenGLBuffer<T, U>::Unbind() const {
    glBindBuffer(m_type, 0);
}

template<BufferType T, typename U>
std::unique_ptr<Buffer<T, U>> Buffer<T, U>::CreateBuffer(const U* data, uint32_t size) {
    return std::move(std::make_unique<OpenGLBuffer<T, U>>(GL_STATIC_DRAW, data, size));
}

}
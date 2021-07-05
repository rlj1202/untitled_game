#include "platforms/luvoasi/graphics/buffer_opengl.h"

namespace Luvoasi {

template<>
OpenGLBuffer<BufferType::ARRAY_BUFFER, float>::OpenGLBuffer(
    GLenum usage, const void* data, uint32_t size)
    : OpenGLBuffer(GL_ARRAY_BUFFER, usage, data, size) {
}

template<>
OpenGLBuffer<BufferType::ELEMENT_ARRAY_BUFFER, unsigned int>::OpenGLBuffer(
    GLenum usage, const void* data, uint32_t size)
    : OpenGLBuffer(GL_ELEMENT_ARRAY_BUFFER, usage, data, size) {
}

template std::unique_ptr<ArrayBuffer> Buffer<BufferType::ARRAY_BUFFER, float>::CreateBuffer(const float* data, uint32_t size);
template std::unique_ptr<IndexBuffer> Buffer<BufferType::ELEMENT_ARRAY_BUFFER, unsigned int>::CreateBuffer(const unsigned int* data, uint32_t size);

}

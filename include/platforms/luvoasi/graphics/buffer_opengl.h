/**
 * @file buffer_opengl.h
 * @author Jisu Sim (rlj1202@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef H_LUVOASI_BUFFER_OPENGL
#define H_LUVOASI_BUFFER_OPENGL

#include "luvoasi/graphics/buffer.h"

#include "platforms/luvoasi/graphics/common_opengl.h"

namespace Luvoasi {

/**
 * @brief
 * @details
 * 
 * @tparam T BufferType
 * @tparam U internal element data type
 */
template<BufferType T, typename U>
class OpenGLBuffer : public Buffer<T, U> {
public:
    OpenGLBuffer(GLenum usage);
    OpenGLBuffer(GLenum usage, const void* data, uint32_t size);
    OpenGLBuffer(GLenum type, GLenum usage, const void* data, uint32_t size);
    OpenGLBuffer(const OpenGLBuffer<T, U>& o) = delete;
    OpenGLBuffer(OpenGLBuffer<T, U>&& o);
    virtual ~OpenGLBuffer();

    OpenGLBuffer<T, U>& operator=(const OpenGLBuffer<T, U>& o) = delete;
    OpenGLBuffer<T, U>& operator=(OpenGLBuffer<T, U>&& o) noexcept;

    uint32_t GetId() const override;

    void SetData(const U* data, uint32_t size) override;

    uint32_t GetLength() const override;

    void Bind() const override;
    void Unbind() const override;

private:
    uint32_t m_id;
    GLenum m_type;
    GLenum m_usage;
    uint32_t m_length;
};

}
#include "platforms/luvoasi/graphics/buffer_opengl.tcc"

#endif
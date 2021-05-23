#include "platforms/luvoasi/graphics/texture_opengl.h"

#include "GLES3/gl3.h"

namespace Luvoasi {

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
    : m_width(width), m_height(height) {
    m_id = 0;
    glGenTextures(1, &m_id);

    // in webgl2, combinations of internal format and format are very limited.
    // https://www.khronos.org/registry/webgl/specs/latest/2.0/#3.7.6
    m_internal_format = GL_RGBA;
    m_format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexStorage2D(GL_TEXTURE_2D, 1, m_internal_format, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

uint32_t OpenGLTexture2D::GetId() const {
    return m_id;
}

uint32_t OpenGLTexture2D::GetWidth() const {
    return m_width;
}

uint32_t OpenGLTexture2D::GetHeight() const {
    return m_height;
}

void OpenGLTexture2D::SetData(void* data, uint32_t size) {
    glBindTexture(GL_TEXTURE_2D, m_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        0, 0,
        m_width, m_height,
        m_format, GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::SetData(
    uint32_t x_offset, uint32_t y_offset,
    uint32_t width, uint32_t height,
    void* data, uint32_t size) {
    glBindTexture(GL_TEXTURE_2D, m_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        x_offset, y_offset,
        width, height,
        m_format, GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::Bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void OpenGLTexture2D::Unbind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}

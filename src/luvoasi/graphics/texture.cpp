#include "luvoasi/graphics/texture.h"

namespace Luvoasi {

SubTexture2D::SubTexture2D(Texture2D* texture)
    : m_texture(texture) {
    
}

uint32_t SubTexture2D::GetId() const {
    return m_texture->GetId();
}

uint32_t SubTexture2D::GetWidth() const {
    return m_width;
}

uint32_t SubTexture2D::GetHeight() const {
    return m_height;
}

void SubTexture2D::SetData(void* data, uint32_t size) {
    m_texture->SetData(
        m_x_offset, m_y_offset,
        m_width, m_height,
        data, size);
}

void SubTexture2D::SetData(uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void* data, uint32_t size) {
    m_texture->SetData(
        m_x_offset + x_offset, m_y_offset + y_offset,
        width, height,
        data, size);
}

void SubTexture2D::Bind(uint32_t slot) const {
    m_texture->Bind(slot);
}

void SubTexture2D::Unbind(uint32_t slot) const {
    m_texture->Unbind(slot);
}

}

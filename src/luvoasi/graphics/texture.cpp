#define LUVOASI_DEBUG_ENABLE

#include "luvoasi/graphics/texture.h"

#include <fstream>
#include <sstream>

// nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

void SubTexture2D::SetData(const void* data, uint32_t size) {
    m_texture->SetData(
        m_x_offset, m_y_offset,
        m_width, m_height,
        data, size);
}

void SubTexture2D::SetData(
    uint32_t x_offset, uint32_t y_offset,
    uint32_t width, uint32_t height,
    const void* data, uint32_t size) {
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

std::unique_ptr<Texture2D> Texture2D::CreateTexture2DFromFile(const char* path) {
    int width;
    int height;
    int channels;
    unsigned char* bytes = stbi_load(path, &width, &height, &channels, 0);
    if (!bytes) {
        LUVOASI_DEBUG_STDOUT("Texture2D::CreateTexture2DFromFile() : failed to load image.\n");
        return nullptr;
    }

    std::unique_ptr<Texture2D> texture = CreateTexture2D(
        width, height, channels, bytes
    );

    stbi_image_free(bytes);

    return std::move(texture);
}

}

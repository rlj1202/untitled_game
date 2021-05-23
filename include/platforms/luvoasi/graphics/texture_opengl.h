#ifndef H_LUVOASI_TEXTURE_OPENGL
#define H_LUVOASI_TEXTURE_OPENGL

#include "luvoasi/graphics/texture.h"

namespace Luvoasi {

class OpenGLTexture2D : public Texture2D {
public:
    OpenGLTexture2D(uint32_t width, uint32_t height);
    virtual ~OpenGLTexture2D();

    uint32_t GetId() const override;

    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;

    void SetData(void* data, uint32_t size) override;
    void SetData(
        uint32_t x_offset, uint32_t y_offset,
        uint32_t width, uint32_t height,
        void* data, uint32_t size) override;

    void Bind(uint32_t slot) const override;
    void Unbind(uint32_t slot) const override;

private:
    uint32_t m_id;
    
    uint32_t m_width, m_height;
    uint32_t m_internal_format, m_format;
};

}

#endif
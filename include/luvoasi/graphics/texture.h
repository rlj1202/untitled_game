#ifndef H_LUVOASI_TEXTURE
#define H_LUVOASI_TEXTURE

#include <glm/glm.hpp>

#include "luvoasi/core/base.h"

namespace Luvoasi {

/**
 * @brief 
 * @details
 * 
 */
class Texture {
public:
    virtual ~Texture() = default;

    /**
     * @brief 
     */
    virtual uint32_t GetId() const = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    /**
     * @brief
     * 
     * @param data
     * @param size
     */
    virtual void SetData(void* data, uint32_t size) = 0;

    /**
     * @param x_offset
     * @param y_offset
     * @param width
     * @param height
     * @param data
     * @param size
     */
    virtual void SetData(
        uint32_t x_offset, uint32_t y_offset,
        uint32_t width, uint32_t height,
        void* data, uint32_t size) = 0;

    /**
     * @param slot
     */
    virtual void Bind(uint32_t slot = 0) const = 0;

    /**
     * @param slot
     */
    virtual void Unbind(uint32_t slot = 0) const = 0;
};

/**
 * @brief
 * @details
 * 
 */
class Texture2D : public Texture {
public:
};

/**
 * @brief
 * @details
 * 
 */
class SubTexture2D : public Texture2D {
public:
    SubTexture2D(Texture2D* texture);

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
    Texture2D* m_texture;

    uint32_t m_x_offset, m_y_offset;
    uint32_t m_width, m_height;
};

}

#endif
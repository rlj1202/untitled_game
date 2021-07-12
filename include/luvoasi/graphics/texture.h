#ifndef H_LUVOASI_TEXTURE
#define H_LUVOASI_TEXTURE

#include <memory>

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
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    /**
     * @brief
     * 
     * @param data
     * @param size
     */
    virtual void SetData(const void* data, uint32_t size) = 0;

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
        const void* data, uint32_t size) = 0;

    /**
     * @brief
     * @details
     * 
     * @param width
     * @param height
     * @param bytes
     */
    static std::unique_ptr<Texture2D> CreateTexture2D(
        uint32_t width, uint32_t height, uint32_t channels,
        const unsigned char* bytes);

    static std::unique_ptr<Texture2D> CreateTexture2DFromFile(const char* path);
};

/**
 * @brief 
 * 
 */
class Texture3D : public Texture {
public:
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetDepth() const = 0;

    virtual void SetData(const void* data, uint32_t size) = 0;

    virtual void SetData(
        uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
        uint32_t width, uint32_t height, uint32_t depth,
        const void* data, uint32_t size) = 0;

    static std::unique_ptr<Texture2D> CreateTexture3D(
        uint32_t width, uint32_t height, uint32_t depth, uint32_t channels,
        const unsigned char* bytes);
};

/**
 * @brief Sub 2d image of another 2d image.
 * @details
 * 
 */
class SubTexture2D : public Texture2D {
public:
    SubTexture2D(Texture2D* texture);

    uint32_t GetId() const override;

    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;

    void SetData(const void* data, uint32_t size) override;
    void SetData(
        uint32_t x_offset, uint32_t y_offset,
        uint32_t width, uint32_t height,
        const void* data, uint32_t size) override;

    void Bind(uint32_t slot) const override;
    void Unbind(uint32_t slot) const override;

private:
    Texture2D* m_texture;

    uint32_t m_x_offset, m_y_offset;
    uint32_t m_width, m_height;
};

}

#endif
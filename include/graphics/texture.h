#ifndef H_TEXTURE
#define H_TEXTURE

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <glm/glm.hpp>

#include <GLES3/gl3.h>

#include "rect_pack.h"

class ITexture {
public:
    virtual unsigned int GetId() = 0;

    virtual unsigned int GetWidth() = 0;
    virtual unsigned int GetHeight() = 0;

    virtual void Bind(unsigned int active = 0) = 0;

    virtual glm::mat4 GetTextureTransformationMatrix() = 0;
};

/**
 * Opengl texture wrapper class.
 */
class Texture : public ITexture {
public:
    Texture(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char* data,
        GLenum internal_format, GLenum format
    );
    Texture(const Texture& o) = delete;
    Texture(Texture&& o);
    ~Texture();

    Texture& operator=(const Texture& o) = delete;
    Texture& operator=(Texture&& o);

    unsigned int GetId() override;

    unsigned int GetWidth() override;
    unsigned int GetHeight() override;

    unsigned char* GetData();

    void SetData(
        unsigned int x, unsigned int y,
        unsigned int width, unsigned int height,
        unsigned char* data
    );

    /**
     * Send data to GPU
     */
    void Bake();

    void Bind(unsigned int active = 0) override;
    void Unbind(unsigned int active = 0);

    glm::mat4 GetTextureTransformationMatrix() override;

private:
    unsigned int width;
    unsigned int height;

    unsigned int channels;
    unsigned char* data;

    GLenum internal_format;
    GLenum format;

    unsigned int texture_id;
};

class SubTexture : public ITexture {
    friend class TextureAtlas;

public:
    SubTexture(std::string name, glm::ivec2 pos, glm::ivec2 size, ITexture* texture);

    unsigned int GetId() override;

    unsigned int GetWidth() override;
    unsigned int GetHeight() override;

    void Bind(unsigned int active = 0) override;

    glm::mat4 GetTextureTransformationMatrix() override;

    /// Unique name of texture in the boundary.
    std::string name;

    /// coordinates of left-top corner of bound in uv coordinate system.
    glm::ivec2 pos;

    /// width and height of bound.
    glm::ivec2 size;

    bool operator<(const SubTexture& o) const;

private:
    ITexture* texture;
};

/**
 * Special texture class which can store diffent types of textures together.
 */
class TextureAtlas : public Texture {
public:
    TextureAtlas(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char* data,
        GLenum internal_format, GLenum format
    );
    TextureAtlas(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char* data,
        GLenum internal_format, GLenum format,
        const std::vector<SubTexture>& sub_textures
    );
    TextureAtlas(std::unique_ptr<Texture> texture, const std::vector<SubTexture>& sub_textures);

    std::vector<SubTexture>& GetSubTextures();
    SubTexture* GetSubTexture(std::string name);

    /**
     * @brief Find appropriate place given width and height.
     * 
     * @return Coordinates of corner of area where the given area is placed.
     */
    glm::ivec2 Claim(unsigned int width, unsigned int height);

    glm::ivec2 Claim(unsigned int width, unsigned int height, std::string name);

    /**
     * @brief Add data into appropriate place.
     * 
     * @return Coordinates of corner of area where the given data is drawn into.
     */
    glm::ivec2 AddData(unsigned int width, unsigned height, unsigned char* data);

private:
    RectPack rect_pack;

    std::vector<SubTexture> sub_textures;
};

#endif
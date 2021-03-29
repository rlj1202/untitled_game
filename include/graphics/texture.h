#ifndef H_TEXTURE
#define H_TEXTURE

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <glm/glm.hpp>

#include <GLES3/gl3.h>

class Texture {
public:
    Texture(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char *data,
        GLenum internal_format, GLenum format
    );
    Texture(const Texture &o) = delete;
    Texture(Texture&& o);
    ~Texture();

    unsigned int GetWidth();
    unsigned int GetHeight();

    unsigned char* GetData();

    void SetData(
        unsigned int x, unsigned int y,
        unsigned int width, unsigned int height,
        unsigned char *data
    );

    /**
     * Send data to GPU
     */
    void Bake();

    void Bind(unsigned int active = 0);
    void Unbind(unsigned int active = 0);
private:
    unsigned int width;
    unsigned int height;

    unsigned int channels;
    unsigned char *data;

    GLenum internal_format;
    GLenum format;

    unsigned int texture_id;
};

class TextureBound {
public:
    std::string name;
    /// coordinates of left-top corner of bound in uv coordinate system.
    glm::vec2 origin;
    /// width and height of bound.
    glm::vec2 size;
};

/**
 * Based on The Skyline Bottom-Left Algorithm.
 */
class TextureAtlas : public Texture {
public:
    TextureAtlas(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char *data,
        GLenum internal_format, GLenum format,
        const std::vector<TextureBound> &bounds
    );

    /**
     * Find appropriate place given width and height.
     */
    TextureBound Claim(int width, int height);

    glm::mat4 GetTextureTransformMatrix(const std::string name);

private:
    std::vector<TextureBound> bounds;

    std::map<std::string, glm::mat4> mats;
};

#endif
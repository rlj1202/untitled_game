#ifndef H_TEXTURE
#define H_TEXTURE

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <glm/glm.hpp>

class Texture {
public:
    Texture(unsigned int texture_id);
    Texture(const Texture &o) = delete;
    Texture(Texture&& o);
    ~Texture();

    void Bind(unsigned int active = 0);
private:
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

class TextureAtlas {
public:
    TextureAtlas(std::unique_ptr<Texture> texture, const std::vector<TextureBound> &bounds);

    void Bind();
    glm::mat4 GetTextureTransformMatrix(const std::string name);

private:
    std::unique_ptr<Texture> texture;
    std::map<std::string, glm::mat4> mats;
};

#endif
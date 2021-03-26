#include "graphics/texture.h"

#include <GLES3/gl3.h>
#include <glm/gtc/matrix_transform.hpp>

Texture::Texture(unsigned int texture_id)
    : texture_id(texture_id) {    
}

Texture::Texture(Texture&& o) {
    texture_id = o.texture_id;

    o.texture_id = 0;
}

Texture::~Texture() {
    if (texture_id) glDeleteTextures(1, &texture_id);
}

void Texture::Bind(unsigned int active) {
    glActiveTexture(GL_TEXTURE0 + active);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

TextureAtlas::TextureAtlas(std::unique_ptr<Texture> tex, const std::vector<TextureBound> &bounds)
    : texture(std::move(tex)) {
    for (const TextureBound &bound : bounds) {
        glm::mat4 mat(1);
        mat = glm::scale(mat, glm::vec3(bound.size.s, bound.size.t, 1));
        mat = glm::translate(mat, glm::vec3(bound.origin.x, bound.origin.y, 0));
        mats[bound.name] = mat;
    }
}

void TextureAtlas::Bind() {
    texture->Bind();
}

glm::mat4 TextureAtlas::GetTextureTransformMatrix(const std::string name) {
    return mats[name];
}
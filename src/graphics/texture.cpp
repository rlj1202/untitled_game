#include "graphics/texture.h"

#include <cstring>
#include <cassert>

#include <GLES3/gl3.h>
#include <glm/gtc/matrix_transform.hpp>

Texture::Texture(
    unsigned int width, unsigned int height, unsigned int channels,
    unsigned char *data,
    GLenum internal_format, GLenum format)
    : width(width), height(height), channels(channels),
      internal_format(internal_format), format(format) {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // or GL_NEAREST or GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    size_t bytes = width * height * channels;

    this->data = new unsigned char[bytes];
    if (!this->data) {
        // error : ouf of memory
    }

    if (data) {
        memcpy(this->data, data, bytes);
    }

    Bake();
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(Texture&& o) {
    width = o.width;
    height = o.height;

    channels = o.channels;
    data = o.data;

    internal_format = o.internal_format;
    format = o.format;

    texture_id = o.texture_id;

    o.data = nullptr;
    o.texture_id = 0;
}

Texture::~Texture() {
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }

    if (data) {
        delete data;
        data = nullptr;
    }
}

unsigned int Texture::GetWidth() {
    return width;
}

unsigned int Texture::GetHeight() {
    return height;
}

unsigned char* Texture::GetData() {
    return data;
}

void Texture::SetData(
    unsigned int x, unsigned int y,
    unsigned int width, unsigned int height,
    unsigned char *data
) {
    assert(data);

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            this->data[x+c + (y+r) * this->width * channels] = data[c + r * width * channels];
        }
    }
}

void Texture::Bake() {
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, channels);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void Texture::Bind(unsigned int active) {
    glActiveTexture(GL_TEXTURE0 + active);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::Unbind(unsigned int active) {
    glActiveTexture(GL_TEXTURE0 + active);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureAtlas::TextureAtlas(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char *data,
        GLenum internal_format, GLenum format,
        const std::vector<TextureBound> &bounds
) : Texture(width, height, channels, data, internal_format, format), bounds(bounds) {
    for (const TextureBound &bound : bounds) {
        glm::mat4 mat(1);
        mat = glm::scale(mat, glm::vec3(bound.size.s, bound.size.t, 1));
        mat = glm::translate(mat, glm::vec3(bound.origin.x, bound.origin.y, 0));
        mats[bound.name] = mat;
    }
}

glm::mat4 TextureAtlas::GetTextureTransformMatrix(const std::string name) {
    return mats[name];
}
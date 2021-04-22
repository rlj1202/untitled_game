#include "graphics/texture.h"

#include <cstring>
#include <cassert>
#include <queue>
#include <algorithm>

#include <GLES3/gl3.h>
#include <glm/gtc/matrix_transform.hpp>

#define DEBUG
#include "debug.h"

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
    memset(this->data, 0, bytes);

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

Texture& Texture::operator=(Texture&& o) {
    width = o.width;
    height = o.height;

    channels = o.channels;
    data = o.data;

    internal_format = o.internal_format;
    format = o.format;

    texture_id = o.texture_id;

    o.data = nullptr;
    o.texture_id = 0;

    return *this;
}

unsigned int Texture::GetId() {
    return texture_id;
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

    int min_width = std::min(width, std::max(this->width - x, (unsigned int) 0));
    int min_height = std::min(height, std::max(this->height - y, (unsigned int) 0));

    assert(x + min_width <= this->width);
    assert(y + min_height <= this->height);

    for (int r = 0; r < min_height; r++) {
        for (int c = 0; c < min_width; c++) {
            for (int i = 0; i < channels; i++) {
                this->data[(x+c)*channels + (y+r)*this->width*channels + i]
                    = data[c*channels + r*width*channels + i];
            }
        }
    }
}

void Texture::Bake() {
    assert(data);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // glPixelStorei(GL_UNPACK_ALIGNMENT, channels);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    DEBUG_STDOUT("Texture baked\n");
}

void Texture::Bind(unsigned int active) {
    glActiveTexture(GL_TEXTURE0 + active);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::Unbind(unsigned int active) {
    glActiveTexture(GL_TEXTURE0 + active);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::mat4 Texture::GetTextureTransformationMatrix() {
    return glm::mat4(1);
}

TextureBound::TextureBound(std::string name, glm::ivec2 pos, glm::ivec2 size, TextureAtlas* texture_atlas)
    : name(name), pos(pos), size(size), texture_atlas(texture_atlas) {
}

unsigned int TextureBound::GetId() {
    if (texture_atlas)
        return texture_atlas->GetId();
    else
        return -1;
}

unsigned int TextureBound::GetWidth() {
    return size.x;
}

unsigned int TextureBound::GetHeight() {
    return size.y;
}

void TextureBound::Bind(unsigned int active) {
    if (texture_atlas) {
        texture_atlas->Bind(active);
    }
}

glm::mat4 TextureBound::GetTextureTransformationMatrix() {
    glm::mat4 mat(1);
    mat = glm::translate(mat, glm::vec3(
        pos.x / (float) texture_atlas->GetWidth(),
        pos.y / (float) texture_atlas->GetHeight(),
        0
    ));
    mat = glm::scale(mat, glm::vec3(
        size.x / (float) texture_atlas->GetWidth(),
        size.y / (float) texture_atlas->GetHeight(),
        0
    ));

    return mat;
}

bool TextureBound::operator<(const TextureBound& o) const {
    return name < o.name;
}

TextureAtlas::TextureAtlas(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char *data,
        GLenum internal_format, GLenum format
) : TextureAtlas(width, height, channels, data, internal_format, format, {}) {
}

TextureAtlas::TextureAtlas(
        unsigned int width, unsigned int height, unsigned int channels,
        unsigned char *data,
        GLenum internal_format, GLenum format,
        const std::vector<TextureBound> &bounds
) : Texture(width, height, channels, data, internal_format, format),
    bounds(bounds),
    rect_pack(width, height)
{
    std::sort(this->bounds.begin(), this->bounds.end());
    for (auto& bound : this->bounds) {
        bound.texture_atlas = this;
    }

    // TODO: Make skyline of rect_pack variable using given bounds.
}

std::vector<TextureBound>& TextureAtlas::GetBounds() {
    return bounds;
}

TextureBound* TextureAtlas::GetBound(std::string name) {
    auto result = std::lower_bound(
        bounds.begin(), bounds.end(),
        TextureBound(name, glm::ivec2(), glm::ivec2(), this)
    );
    if (result == bounds.end()) {
        return nullptr;
    }

    return result.base();
}

glm::ivec2 TextureAtlas::Claim(unsigned int width, unsigned int height) {
    return rect_pack.Claim(width, height);
}

glm::ivec2 TextureAtlas::Claim(unsigned int width, unsigned int height, std::string name) {
    glm::ivec2 pos = rect_pack.Claim(width, height);

    bounds.push_back(TextureBound(name, pos, glm::ivec2(width, height), this));

    return pos;
}

glm::ivec2 TextureAtlas::AddData(unsigned int width, unsigned int height, unsigned char* data) {
    glm::ivec2 pos = rect_pack.Claim(width, height);

    if (pos.x != -1 && pos.y != -1) {
        SetData(pos.x, pos.y, width, height, data);
    }

    return pos;
}

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

SubTexture::SubTexture(std::string name, glm::ivec2 pos, glm::ivec2 size, ITexture* texture)
    : name(name), pos(pos), size(size), texture(texture) {
}

unsigned int SubTexture::GetId() {
    if (texture)
        return texture->GetId();
    else
        return -1;
}

unsigned int SubTexture::GetWidth() {
    return size.x;
}

unsigned int SubTexture::GetHeight() {
    return size.y;
}

void SubTexture::Bind(unsigned int active) {
    if (texture) {
        texture->Bind(active);
    }
}

glm::mat4 SubTexture::GetTextureTransformationMatrix() {
    glm::mat4 mat(1);
    mat = glm::translate(mat, glm::vec3(
        pos.x / (float) texture->GetWidth(),
        pos.y / (float) texture->GetHeight(),
        0
    ));
    mat = glm::scale(mat, glm::vec3(
        size.x / (float) texture->GetWidth(),
        size.y / (float) texture->GetHeight(),
        0
    ));

    return mat;
}

bool SubTexture::operator<(const SubTexture& o) const {
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
        const std::vector<SubTexture> &sub_textures
) : Texture(width, height, channels, data, internal_format, format),
    sub_textures(sub_textures),
    rect_pack(width, height)
{
    // to use binary search algorithm.
    std::sort(this->sub_textures.begin(), this->sub_textures.end());
    for (auto& bound : this->sub_textures) {
        bound.texture = this;
    }

    // TODO: Make skyline of rect_pack variable using given bounds.
}

TextureAtlas::TextureAtlas(
    std::unique_ptr<Texture> texture,
    const std::vector<SubTexture>& sub_textures
) : Texture(std::move(*texture.get())),
    sub_textures(sub_textures),
    rect_pack(texture->GetWidth(), texture->GetHeight()) {
    
    // to use binary search algorithm.
    std::sort(this->sub_textures.begin(), this->sub_textures.end());
    for (auto& bound : this->sub_textures) {
        bound.texture = this;
    }
}

std::vector<SubTexture>& TextureAtlas::GetSubTextures() {
    return sub_textures;
}

SubTexture* TextureAtlas::GetSubTexture(std::string name) {
    auto result = std::lower_bound(
        sub_textures.begin(), sub_textures.end(),
        SubTexture(name, glm::ivec2(), glm::ivec2(), this)
    );
    if (result == sub_textures.end()) {
        return nullptr;
    }

    return result.base();
}

glm::ivec2 TextureAtlas::Claim(unsigned int width, unsigned int height) {
    return rect_pack.Claim(width, height);
}

glm::ivec2 TextureAtlas::Claim(unsigned int width, unsigned int height, std::string name) {
    glm::ivec2 pos = rect_pack.Claim(width, height);

    sub_textures.push_back(SubTexture(name, pos, glm::ivec2(width, height), this));

    return pos;
}

glm::ivec2 TextureAtlas::AddData(unsigned int width, unsigned int height, unsigned char* data) {
    glm::ivec2 pos = rect_pack.Claim(width, height);

    if (pos.x != -1 && pos.y != -1) {
        SetData(pos.x, pos.y, width, height, data);
    }

    return pos;
}

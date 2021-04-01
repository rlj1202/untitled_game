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

TextureAtlasNode::TextureAtlasNode() : TextureAtlasNode(0, 0, nullptr) {
}
TextureAtlasNode::TextureAtlasNode(int x, int y, TextureAtlasNode *prev)
    : x(x), y(y), next(nullptr), prev(prev) {
}
TextureAtlasNode::~TextureAtlasNode() {
    if (next) {
        delete next;
        next = nullptr;
    }
    prev = nullptr;
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
) : Texture(width, height, channels, data, internal_format, format), bounds(bounds) {
    head = new TextureAtlasNode();
    head->next = new TextureAtlasNode(0, 0, head);

    assert(head);
    assert(head->next);
    assert(head->next->prev);

    for (const TextureBound &bound : bounds) {
        glm::mat4 mat(1);
        mat = glm::scale(mat, glm::vec3(bound.size.s, bound.size.t, 1));
        mat = glm::translate(mat, glm::vec3(bound.origin.x, bound.origin.y, 0));
        mats[bound.name] = mat;
    }
}
TextureAtlas::TextureAtlas(TextureAtlas &&o) : Texture(std::move(o)) {
    head = o.head;
    bounds = o.bounds;
    mats = o.mats;

    o.head = nullptr;

    DEBUG_STDOUT("TextureAtlas moved\n");
}
TextureAtlas::~TextureAtlas() {
    if (head) {
        delete head;
        head = nullptr;
    }

    DEBUG_STDOUT("TextureAtlas destructed\n");
}

glm::ivec2 TextureAtlas::Claim(int width, int height) {
    assert(width <= this->GetWidth());
    assert(height <= this->GetHeight());
    
    TextureAtlasNode *best_node = nullptr;
    glm::ivec2 result = FindBottomLeft(width, height, &best_node);

    if (best_node && result.y + height <= this->GetHeight()) {
        TextureAtlasNode *new_node = new TextureAtlasNode(result.x, result.y + height, nullptr);
        assert(new_node);

        TextureAtlasNode *prev = best_node->prev;
        assert(prev);
        prev->next = new_node;
        new_node->prev = prev;
        
        TextureAtlasNode *cur = best_node;
        while (cur) {
            int right = cur->next ? cur->next->x : this->GetWidth();
            if (right >= new_node->x + width) {
                break;
            }
            DEBUG_STDOUT("delete node : (%d %d), right = %d\n", cur->x, cur->y, right);
            TextureAtlasNode *next = cur->next;
            cur->next = nullptr;
            cur->prev = nullptr;
            delete cur;
            cur = next;
        }

        new_node->next = cur;
        if (cur) cur->prev = new_node;

        if (cur) cur->x = new_node->x + width;

        return result;
    }

    // TextureAtlasNode *test = head->next;
    // int chain_length = 0;
    // while (test) {
    //     chain_length++;
    //     test = test->next;
    // }
    // DEBUG_STDOUT("chain length : %d\n", chain_length);

    return {-1, -1};
}

glm::ivec2 TextureAtlas::FindBottomLeft(int width, int height, TextureAtlasNode **best_node) {
    assert(width <= this->GetWidth());
    assert(height <= this->GetHeight());

    TextureAtlasNode* cur = head->next;
    assert(cur);
    assert(cur->prev);

    int best_y = this->GetHeight(); // init to maximum value

    *best_node = nullptr;

    while (cur) {
        if (this->GetWidth() - cur->x < width) break;
        
        int min_y = FindMinY(cur, width);

        if (best_y > min_y) {
            best_y = min_y;
            *best_node = cur;
        }

        cur = cur->next;
    }

    DEBUG_STDOUT("FindBottomLeft: (%d %d)\n", width, height);
    if (*best_node) DEBUG_STDOUT("\t(%d %d)\n", (*best_node)->x, (*best_node)->y);

    if (*best_node) return {(*best_node)->x, best_y};
    else return {-1, -1};
}

int TextureAtlas::FindMinY(TextureAtlasNode *node, int width) {
    assert(node);
    assert(width <= this->GetWidth());

    int min_y = 0;
    int left = node->x;
    int right = left + width;

    TextureAtlasNode* cur = node;

    while (cur) {
        if (cur->x < right) {
            if (min_y < cur->y) {
                min_y = cur->y;
            }
        } else {
            break;
        }

        cur = cur->next;
    }

    // DEBUG_STDOUT("FindMinY: %d for width %d\n", min_y, width);
    
    return min_y;
}

glm::mat4 TextureAtlas::GetTextureTransformMatrix(const std::string name) {
    return mats[name];
}
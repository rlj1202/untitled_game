#ifndef H_FONT
#define H_FONT

#include "graphics/texture.h"

class Glyph {
    uint32_t codepoint;
    unsigned int width;
    unsigned int height;
    unsigned int offset_x;
    unsigned int offset_y;
    float advance_x;
    float advance_y;
};

class Font {
public:
    Font();

private:

};

class FontTextureAtlas {
public:
    FontTextureAtlas();

    

    Texture& GetTexture();

private:
    std::unique_ptr<Texture> texture;
};

#endif
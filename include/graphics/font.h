#ifndef H_FONT
#define H_FONT

#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics/texture.h"
#include "graphics/mesh.h"

struct Glyph {
    uint32_t codepoint;
    uint32_t glyph_index;
    
    unsigned int offset_x;
    unsigned int offset_y;

    unsigned int width;
    unsigned int height;
    int bearing_x;
    int bearing_y;
    float advance_x;
    float advance_y;
};

class FontFace {
    friend class FontLibrary;
    friend class FontRenderer;
private:
    FontFace(FT_Face ft_face);

public:
    void SetPixelSize(int width, int height);

    unsigned int GetCharIndex(unsigned long codepoint);
    bool LoadGlyph(unsigned int glyph_index, int flags);
    bool LoadChar(unsigned long codepoint, int flags);

    glm::vec2 GetKerning(uint32_t left, uint32_t right, int flags);

    bool RenderGlyph();

private:
    FT_Face ft_face;
};

class FontLibrary {
public:

public:
    FontLibrary();
    FontLibrary(const FontLibrary &o) = delete;
    ~FontLibrary();

    FontFace NewFontFace(std::string path, int face_index = 0);

private:
    FT_Library ft_library;
};

class FontRenderer {
public:
    FontRenderer(FontFace face);

    void Render(int x, int y, std::wstring text);

    Texture* GetTexture();

private:
    bool LoadGlyph(unsigned int codepoint);

    FontFace face;
    std::unordered_map<unsigned int, Glyph> glyph_infos;

    MeshProfile quad_profile;
    std::unique_ptr<TextureAtlas> texture_atlas;
};

#endif
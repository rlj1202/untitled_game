#ifndef H_LUVOASI_FONT
#define H_LUVOASI_FONT

#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>

#include "luvoasi/core/base.h"
#include "luvoasi/graphics/texture.h"
#include "luvoasi/utils/rect_pack.h"

namespace Luvoasi {

/**
 * @brief Contains informations about glyph.
 * 
 */
struct Glyph {
    /// codepoint of glyph in unicode namespace.
    uint32_t codepoint;
    /// 
    uint32_t glyph_index;
    
    /// in texture
    unsigned int offset_x;
    /// in texture
    unsigned int offset_y;

    unsigned int width;
    unsigned int height;
    int bearing_x;
    int bearing_y;
    float advance_x;
    float advance_y;
};

/**
 * @brief 
 * 
 */
class FontFace {
public:
    FontFace(FT_Face ft_face);
    FontFace(const FontFace& o) = delete;
    FontFace(FontFace&& o);
    ~FontFace();

    FontFace& operator=(const FontFace& o) = delete;
    FontFace& operator=(FontFace&& o) noexcept;

    void SetPixelSize(int width, int height);

    unsigned int GetCharIndex(unsigned long codepoint) const;
    bool LoadGlyph(unsigned int glyph_index, int flags) const;
    bool LoadChar(unsigned long codepoint, int flags) const;

    glm::vec2 GetKerning(uint32_t left, uint32_t right, int flags) const;

    bool RenderGlyph() const;

    FT_Face m_ft_face;
};

/**
 * @brief 
 * 
 */
class FontLibrary {
public:
    FontLibrary();
    FontLibrary(const FontLibrary& o) = delete;
    FontLibrary(FontLibrary&& o);
    ~FontLibrary();

    FontLibrary& operator=(const FontLibrary& o) = delete;
    FontLibrary& operator=(FontLibrary&& o) noexcept;

    FontFace NewFontFace(const std::string& path, int face_index = 0) const;

private:
    FT_Library m_ft_library;
};

/**
 * @brief 
 * 
 */
class BitmapFont {
public:
    BitmapFont(const FontFace* fontface, uint32_t width, uint32_t height);

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

    Texture* GetTexture(uint32_t codepoint);

    const Glyph* GetGlyphInfo(uint32_t codepoint);

    /**
     * @brief Load glyph into the texture.
     * 
     * @param codepoint 
     * @return true The glyph is successfully loaded to the existing texture.
     * @return false The glyph is not loaded due to lack of space or
     *         lack of glyph itself corresponding to codepoint. But even though
     *         the glyph is not loaded, the glyph informations could be loaded.
     *         a space (' ') is the case.
     */
    bool LoadGlyph(uint32_t codepoint);

private:
    const FontFace* m_fontface;
    uint32_t m_width;
    uint32_t m_height;
    RectPack m_rect_pack;
    std::unique_ptr<Texture2D> m_texture;
    std::unordered_map<uint32_t, Glyph> m_glyph_infos;
};

}

#endif
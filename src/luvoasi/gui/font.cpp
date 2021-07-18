#include "luvoasi/gui/font.h"

namespace Luvoasi {

/*******************************************************************************
 * FontFace
 ******************************************************************************/

FontFace::FontFace(FT_Face ft_face) : m_ft_face(ft_face) {
}

FontFace::FontFace(FontFace&& o) {
    m_ft_face = o.m_ft_face;
    o.m_ft_face = nullptr;
}

FontFace::~FontFace() {
    if (m_ft_face) { // TODO:
        m_ft_face = nullptr;
    }
}

FontFace& FontFace::operator=(FontFace&& o) noexcept {
    if (this == &o) return *this;

    m_ft_face = o.m_ft_face;
    o.m_ft_face = nullptr;

    return *this;
}

void FontFace::SetPixelSize(int width, int height) {
    LUVOASI_ASSERT(m_ft_face);

    FT_Error error;
    error = FT_Set_Pixel_Sizes(m_ft_face, width, height);
    if (error) {
        LUVOASI_DEBUG_STDOUT("Failed to set pixel sizes\n");
    }
}

unsigned int FontFace::GetCharIndex(unsigned long codepoint) const {
    unsigned int glyph_index = FT_Get_Char_Index(m_ft_face, codepoint);

    return glyph_index;
}

bool FontFace::LoadGlyph(unsigned int glyph_index, int flags) const {
    FT_Error error;
    error = FT_Load_Glyph(m_ft_face, glyph_index, flags);
    if (error) {
        return false;
    }

    return true;
}

bool FontFace::LoadChar(unsigned long codepoint, int flags) const {
    LUVOASI_ASSERT(m_ft_face);

    FT_Error error;
    error = FT_Load_Char(m_ft_face, codepoint, flags);
    if (error) {
        return false;
    }

    return true;
}

glm::vec2 FontFace::GetKerning(uint32_t left, uint32_t right, int flags) const {
    FT_Vector delta;
    FT_Get_Kerning(m_ft_face, left, right, flags, &delta);

    return glm::vec2(delta.x, delta.y);
}

bool FontFace::RenderGlyph() const {
    LUVOASI_ASSERT(m_ft_face);
    LUVOASI_ASSERT(m_ft_face->glyph);

    FT_Error error;
    error = FT_Render_Glyph(m_ft_face->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        return false;
    }

    return true;
}

/*******************************************************************************
 * FontLibrary
 ******************************************************************************/

FontLibrary::FontLibrary() : m_ft_library(nullptr) {
    FT_Error error;
    error = FT_Init_FreeType(&m_ft_library);
    if (error) { // TODO: 

    }
}

FontLibrary::FontLibrary(FontLibrary&& o) {
    m_ft_library = o.m_ft_library;
    o.m_ft_library = nullptr;
}

FontLibrary::~FontLibrary() {
    if (m_ft_library) {
        FT_Error error;
        error = FT_Done_FreeType(m_ft_library);
        if (error) { // TODO: 

        }
    }
}

FontLibrary& FontLibrary::operator=(FontLibrary&& o) noexcept {
    if (this == &o) return *this;

    m_ft_library = o.m_ft_library;
    o.m_ft_library = nullptr;

    return *this;
}

FontFace FontLibrary::NewFontFace(const std::string& path, int face_index) const {
    LUVOASI_ASSERT(m_ft_library);

    FT_Error error;
    FT_Face ft_face = nullptr;
    error = FT_New_Face(m_ft_library, path.c_str(), face_index, &ft_face);
    if (error == FT_Err_Unknown_File_Format) {
        LUVOASI_DEBUG_STDOUT("Unknown font file format.\n");
    } else if (error) {
        LUVOASI_DEBUG_STDOUT("Failed to load font file.\n");
    }

    return FontFace(ft_face);
}

/*******************************************************************************
 * BitmapFont
 ******************************************************************************/

BitmapFont::BitmapFont(const FontFace* fontface, uint32_t width, uint32_t height)
    : m_width(width), m_height(height),
      m_fontface(fontface), m_rect_pack(width, height) {

    m_texture = std::move(Texture2D::CreateTexture2D(width, height, 2, nullptr));

    /*
    for (unsigned int codepoint = 0; codepoint < 128; codepoint++) {
        LoadGlyph(codepoint);
    }
    for (unsigned int codepoint = U'가'; codepoint <= U'힣'; codepoint++) {
        LoadGlyph(codepoint);
    }
    */
}

uint32_t BitmapFont::GetWidth() const {
    return m_width;
}

uint32_t BitmapFont::GetHeight() const {
    return m_height;
}

Texture* BitmapFont::GetTexture(uint32_t codepoint) {
    return m_texture.get();
}

const Glyph* BitmapFont::GetGlyphInfo(uint32_t codepoint) {
    auto it = m_glyph_infos.find(codepoint);
    if (it == m_glyph_infos.end()) {
        return nullptr;
    }
    return &it->second;
}

bool BitmapFont::LoadGlyph(uint32_t codepoint) {
    // check the glyph is already loaded.
    if (m_glyph_infos.count(codepoint)) return false;

    unsigned int glyph_index = m_fontface->GetCharIndex(codepoint);

    if (!m_fontface->LoadGlyph(glyph_index, FT_LOAD_RENDER)) {
        LUVOASI_DEBUG_STDOUT("Failed to load glyph u+%04x.\n", codepoint);
        return false;
    }
    FT_GlyphSlot glyph = m_fontface->m_ft_face->glyph;

    Glyph glyph_info;
    glyph_info.offset_x = 0;
    glyph_info.offset_y = 0;
    glyph_info.width = 0;
    glyph_info.height = 0;
    glyph_info.codepoint = codepoint;
    glyph_info.glyph_index = glyph_index;
    glyph_info.bearing_x = glyph->bitmap_left;
    glyph_info.bearing_y = glyph->bitmap_top;
    glyph_info.advance_x = glyph->advance.x / 64.0f; // 1/64th of pixels.
    glyph_info.advance_y = glyph->advance.y / 64.0f;
    m_glyph_infos[codepoint] = glyph_info;

    if (!m_fontface->RenderGlyph()) {
        LUVOASI_DEBUG_STDOUT("FontRenderer : Failed to render glyph u+%04x.\n", codepoint);
        return false;
    }
    FT_Bitmap &bitmap = glyph->bitmap;
    if (!bitmap.buffer) {
        LUVOASI_DEBUG_STDOUT("No bitmap data for u+%04x.\n", codepoint);
        return false;
    }

    int padding = 1;
    glm::ivec2 pos = m_rect_pack.Claim(bitmap.width + padding, bitmap.rows + padding);
    if (pos.x == -1 || pos.y == -1) {
        LUVOASI_DEBUG_STDOUT("no more space for glyph u+%04x.\n", codepoint);
        return false;
    }

    glyph_info.offset_x = pos.x;
    glyph_info.offset_y = pos.y;
    glyph_info.width = bitmap.width;
    glyph_info.height = bitmap.rows;
    m_glyph_infos[codepoint] = glyph_info;

    unsigned char* data = new unsigned char[bitmap.width * bitmap.rows * 2];
    for (int r = 0; r < bitmap.rows; r++) {
        for (int c = 0; c < bitmap.width; c++) {
            int index = (c + r*bitmap.width) * 2;

            // Luminance
            data[index + 0] = 0xff;
            // Alpha
            data[index + 1] = bitmap.buffer[c + r * bitmap.pitch];
        }
    }

    m_texture->SetData(
        pos.x, pos.y,
        bitmap.width, bitmap.rows,
        data, bitmap.width * bitmap.rows * 2);
    
    delete[] data;

    return true;
}

}

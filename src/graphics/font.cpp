#include "graphics/font.h"

#include <cassert>

#include "graphics/mesh_builder.h"

#define DEBUG
#include "debug.h"

FontFace::FontFace(FT_Face ft_face) : ft_face(ft_face) {
}

void FontFace::SetPixelSize(int width, int height) {
    assert(ft_face);

    FT_Error error;
    error = FT_Set_Pixel_Sizes(ft_face, width, height);
    if (error) {
        DEBUG_STDOUT("Failed to set pixel sizes\n");
    }
}

unsigned int FontFace::GetCharIndex(unsigned long codepoint) {
    unsigned int glyph_index = FT_Get_Char_Index(ft_face, codepoint);
    return glyph_index;
}

bool FontFace::LoadGlyph(unsigned int glyph_index, int flags) {
    FT_Error error;
    error = FT_Load_Glyph(ft_face, glyph_index, flags);
    if (error) {
        return false;
    }

    return true;
}

bool FontFace::LoadChar(unsigned long codepoint, int flags) {
    assert(ft_face);

    FT_Error error;
    error = FT_Load_Char(ft_face, codepoint, flags);
    if (error) {
        return false;
    }

    return true;
}

glm::vec2 FontFace::GetKerning(uint32_t left, uint32_t right, int flags) {
    FT_Vector delta;
    FT_Get_Kerning(ft_face, left, right, flags, &delta);

    return glm::vec2(delta.x, delta.y);
}

bool FontFace::RenderGlyph() {
    assert(ft_face);
    assert(ft_face->glyph);

    FT_Error error;
    error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        return false;
    }

    return true;
}

FontLibrary::FontLibrary() {
    FT_Error error;
    error = FT_Init_FreeType(&ft_library);
    if (error) {
        DEBUG_STDOUT("Failed to init freetype\n");
    }
}

FontLibrary::~FontLibrary() {
    if (ft_library) {
        FT_Error error;
        error = FT_Done_FreeType(ft_library);
        if (error) {
        
        }
    }
}

FontFace FontLibrary::NewFontFace(std::string path, int face_index) {
    assert(ft_library);

    FT_Error error;
    FT_Face ft_face = nullptr;
    error = FT_New_Face(ft_library, path.c_str(), face_index, &ft_face);
    if (error == FT_Err_Unknown_File_Format) {
        DEBUG_STDOUT("Unknown font file format.\n");
    } else if (error) {
        DEBUG_STDOUT("Failed to load font file.\n");
    }

    return FontFace(ft_face);
}

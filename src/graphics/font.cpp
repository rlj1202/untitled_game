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

FontRenderer::FontRenderer(FontFace face) : face(face) {
    assert(face.ft_face);

    face.SetPixelSize(0, 16);
    texture_atlas = std::make_unique<TextureAtlas>(
        2048, 2048, 2,
        nullptr,
        GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA
    );

    for (unsigned int codepoint = 0; codepoint < 128; codepoint++) {
        LoadGlyph(codepoint);
    }
    for (unsigned int codepoint = U'가'; codepoint <= U'힣'; codepoint++) {
        LoadGlyph(codepoint);
    }

    texture_atlas->Bake();

    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
    };
    quad_profile = MeshProfile(vertices, indices, "");
}

void FontRenderer::Render(int x, int y, std::wstring text) {
    float cur_x = 0;
    uint32_t prev_glyph_index = 0;

    for (wchar_t codepoint : text) {
        if (!glyph_infos.count(codepoint)) continue;

        Glyph& info = glyph_infos[codepoint];

        if (prev_glyph_index) {
            glm::vec2 kerning = face.GetKerning(prev_glyph_index, info.glyph_index, FT_KERNING_DEFAULT);
            kerning = kerning / 64.0f;

            cur_x += kerning.x;
        }

        profile.Append(
            quad_profile
                .Scale(glm::vec3(info.width, info.height, 0))
                .Translate(glm::vec3(
                    x + info.bearing_x + cur_x,
                    y + (int) info.height - info.bearing_y,
                    0.0f
                ))
                .TexScale(glm::vec2(
                    (float) info.width,
                    (float) info.height
                ))
                .TexTranslate(glm::vec2(
                    (float) info.offset_x,
                    (float) info.offset_y
                ))
                .TexScale(glm::vec2(
                    1.0f / texture_atlas->GetWidth(),
                    1.0f / texture_atlas->GetHeight()
                ))
        );

        cur_x += info.advance_x;
    }
}

void FontRenderer::Flush() {
    Mesh mesh = BuildMesh(profile);

    texture_atlas->Bind();
    mesh.Draw();

    profile = MeshProfile();
}

Texture* FontRenderer::GetTexture() {
    return texture_atlas.get();
}

bool FontRenderer::LoadGlyph(unsigned int codepoint) {
    unsigned int glyph_index = face.GetCharIndex(codepoint);

    if (!face.LoadGlyph(glyph_index, FT_LOAD_RENDER)) {
        DEBUG_STDOUT("Failed to load glyph u+%04x.\n", codepoint);
        return false;
    }
    FT_GlyphSlot glyph = face.ft_face->glyph;

    Glyph glyph_info;
    glyph_info.offset_x = 0;
    glyph_info.offset_y = 0;
    glyph_info.width = 0;
    glyph_info.height = 0;
    glyph_info.codepoint = codepoint;
    glyph_info.glyph_index = glyph_index;
    glyph_info.bearing_x = glyph->bitmap_left;
    glyph_info.bearing_y = glyph->bitmap_top;
    glyph_info.advance_x = glyph->advance.x / 64.0f;
    glyph_info.advance_y = glyph->advance.y / 64.0f;
    glyph_infos[codepoint] = glyph_info;

    if (!face.RenderGlyph()) {
        DEBUG_STDOUT("Failed to render glyph u+%04x.\n", codepoint);
        return false;
    }
    FT_Bitmap &bitmap = glyph->bitmap;
    if (!bitmap.buffer) {
        // DEBUG_STDOUT("No bitmap data for u+%04x.\n", charcode);
        return false;
    }

    int padding = 1;
    glm::ivec2 pos = texture_atlas->Claim(bitmap.width + padding, bitmap.rows + padding);
    if (pos.x == -1 || pos.y == -1) {
        // DEBUG_STDOUT("no more space for glyph u+%04x.\n", charcode);
        return false;
    }

    glyph_info.offset_x = pos.x;
    glyph_info.offset_y = pos.y;
    glyph_info.width = bitmap.width;
    glyph_info.height = bitmap.rows;
    glyph_infos[codepoint] = glyph_info;

    unsigned char* data = new unsigned char[bitmap.width * bitmap.rows * 2];
    for (int r = 0; r < bitmap.rows; r++) {
        for (int c = 0; c < bitmap.width; c++) {
            data[c*2 + r * bitmap.width * 2 + 0] = 255;
            data[c*2 + r * bitmap.width * 2 + 1] = bitmap.buffer[c + r * bitmap.pitch];
        }
    }

    texture_atlas->SetData(pos.x, pos.y, bitmap.width, bitmap.rows, data);
    
    delete[] data;
    
    return true;
}
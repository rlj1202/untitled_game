#include "graphics/canvas.h"

#include "graphics/mesh_builder.h"

Canvas::Canvas() {
    font_library = std::make_unique<FontLibrary>();
    font_face = std::make_unique<FontFace>(font_library->NewFontFace(
        "/res/D2Coding-Ver1.3.2-20180524-all.ttc"
        // "/res/NanumGothic.ttf"
    ));

    font_face->SetPixelSize(0, 16);
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

    {
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
        font_quad_profile = MeshProfile(vertices, indices, texture_atlas.get());
    }
    {
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3,
        };
        quad_profile = MeshProfile(vertices, indices, nullptr); // TODO:
    }
}

void Canvas::Draw(Texture* texture, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    mesh_profiles[texture].Append(vertices, indices);
}

void Canvas::Draw(Texture* texture, MeshProfile& profile) {
    mesh_profiles[texture].Append(profile);
}

void Canvas::DrawRect(Texture* texture, glm::vec3 pos, glm::vec2 size) {
    MeshProfile profile = quad_profile
        .Clone()
        .Scale(glm::vec3(size, 0.0f))
        .Translate(pos);
    Draw(texture, profile);
}

int Canvas::DrawText(glm::vec3 pos, std::wstring text, int max_width) {
    float cur_x = 0;
    uint32_t prev_glyph_index = 0;

    for (int i = 0; i < text.size(); i++) {
        wchar_t codepoint = text[i];
        if (!glyph_infos.count(codepoint)) continue;

        Glyph& info = glyph_infos[codepoint];

        if (prev_glyph_index) {
            glm::vec2 kerning = font_face->GetKerning(prev_glyph_index, info.glyph_index, FT_KERNING_DEFAULT);
            kerning = kerning / 64.0f;

            cur_x += kerning.x;
        }

        if (info.bearing_x + cur_x + info.advance_x >= max_width) {
            return i;
        }

        MeshProfile profile = font_quad_profile
            .Clone()
            .Scale(glm::vec3(info.width, info.height, 0))
            .Translate(glm::vec3(
                pos.x + info.bearing_x + cur_x,
                pos.y + (int) info.height - info.bearing_y,
                pos.z
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
            ));
        Draw((Texture*) texture_atlas.get(), profile);

        cur_x += info.advance_x;
    }

    return text.size();
}

void Canvas::Render() {
    for (auto key : mesh_profiles) {
        key.first->Bind();
        mesh = std::make_unique<Mesh>(BuildMesh(key.second));
        mesh->Draw();
    }
}

void Canvas::Clear() {
    for (auto& pair : mesh_profiles) {
        pair.second.Clear();
    }
}

void Canvas::Flush() {
    Render();
    Clear();
}

bool Canvas::LoadGlyph(unsigned int codepoint) {
    unsigned int glyph_index = font_face->GetCharIndex(codepoint);

    if (!font_face->LoadGlyph(glyph_index, FT_LOAD_RENDER)) {
        DEBUG_STDOUT("Failed to load glyph u+%04x.\n", codepoint);
        return false;
    }
    FT_GlyphSlot glyph = font_face->ft_face->glyph;

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

    if (!font_face->RenderGlyph()) {
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

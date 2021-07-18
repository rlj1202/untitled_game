#include "luvoasi/rendering/renderer.h"

namespace Luvoasi {

/*******************************************************************************
 * Renderer
 ******************************************************************************/



/*******************************************************************************
 * FontRenderer
 ******************************************************************************/

FontRenderer::FontRenderer() {
    m_font_face = std::make_unique<FontFace>(std::move(m_font_lib.NewFontFace("/res/D2Coding-Ver1.3.2-20180524-all.ttc")));
    m_font_face->SetPixelSize(0, 16);
    m_bitmap_font = std::make_unique<BitmapFont>(m_font_face.get(), 2048, 2048);
}

int FontRenderer::DrawString(const std::wstring& string, float x, float y, float scale, float max_width) {
    float cur_length = 0;
    int cur_index = 0;
    uint32_t prev_glyph_index = 0;
    unsigned int last_vertex = m_vertices.size() / 5;

    for (; cur_index < string.size(); cur_index++) {
        wchar_t codepoint = string[cur_index];

        const Glyph* glyph = m_bitmap_font->GetGlyphInfo(codepoint);
        if (!glyph) {
            if (m_bitmap_font->LoadGlyph(codepoint)) {
                glyph = m_bitmap_font->GetGlyphInfo(codepoint);

                LUVOASI_DEBUG_STDOUT("FontRenderer : successfully load glyph for %d.\n", codepoint);
            } else {
                // TODO: just skip or draw something else?
                continue;
            }
        }

        // kerning
        if (prev_glyph_index) {
            glm::vec2 kerning = m_font_face->GetKerning(prev_glyph_index, glyph->glyph_index, FT_KERNING_DEFAULT);
            kerning /= 64.0f;

            cur_length += kerning.x;
        }

        // Check max width if the argument is greater than zero.
        if (max_width > 0 && cur_length + glyph->bearing_x + glyph->advance_x >= max_width) {
            break;
        }

        // Draw string
        m_vertices.reserve(m_vertices.size() + 5 * 4);
        m_indices.reserve(m_indices.size() + 3 * 2);

        float pos_x = x + glyph->bearing_x * scale + cur_length;
        float pos_y = y - glyph->bearing_y * scale;

        m_vertices.insert(m_vertices.end(), {
            pos_x + 0.0f * glyph->width * scale,
            pos_y + 1.0f * glyph->height * scale,
            0.0f,
            (glyph->offset_x + 0.0f * glyph->width) / (float) m_bitmap_font->GetWidth(),
            (glyph->offset_y + 1.0f * glyph->height) / (float) m_bitmap_font->GetHeight(),
            
            pos_x + 1.0f * glyph->width * scale,
            pos_y + 1.0f * glyph->height * scale,
            0.0f,
            (glyph->offset_x + 1.0f * glyph->width) / (float) m_bitmap_font->GetWidth(),
            (glyph->offset_y + 1.0f * glyph->height) / (float) m_bitmap_font->GetHeight(),
            
            pos_x + 1.0f * glyph->width * scale,
            pos_y + 0.0f * glyph->height * scale,
            0.0f,
            (glyph->offset_x + 1.0f * glyph->width) / (float) m_bitmap_font->GetWidth(),
            (glyph->offset_y + 0.0f * glyph->height) / (float) m_bitmap_font->GetHeight(),
            
            pos_x + 0.0f * glyph->width * scale,
            pos_y + 0.0f * glyph->height * scale,
            0.0f,
            (glyph->offset_x + 0.0f * glyph->width) / (float) m_bitmap_font->GetWidth(),
            (glyph->offset_y + 0.0f * glyph->height) / (float) m_bitmap_font->GetHeight(),
        });

        m_indices.insert(m_indices.end(), {
            last_vertex + 0, last_vertex + 1, last_vertex + 2,
            last_vertex + 0, last_vertex + 2, last_vertex + 3,
        });

        // Advance cursor
        cur_length += glyph->advance_x * scale;
        prev_glyph_index = glyph->glyph_index;
        last_vertex += 4;
    }

    return cur_index;
}

void FontRenderer::Render() {
    auto array_buffer = ArrayBuffer::CreateBuffer(m_vertices.data(), m_vertices.size());
    auto index_buffer = IndexBuffer::CreateBuffer(m_indices.data(), m_indices.size());

    m_va = VertexArray::CreateVertexArray();
    m_va->AttachArrayBuffer(std::move(array_buffer), {
        BufferElement{ 0, 3, false, DataType::FLOAT32 },
        BufferElement{ 1, 2, false, DataType::FLOAT32 },
    });
    m_va->AttachIndexBuffer(std::move(index_buffer));

    m_bitmap_font->GetTexture(0)->Bind();
    m_va->Draw();

    m_vertices.clear();
    m_indices.clear();
    m_vertices.reserve(512);
    m_indices.reserve(512);
}

BitmapFont* FontRenderer::GetBitmapFont() const {
    return m_bitmap_font.get();
}

}
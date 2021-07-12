#ifndef H_LUVOASI_RENDERER
#define H_LUVOASI_RENDERER

#include <string>
#include <vector>

#define LUVOASI_DEBUG_ENABLE
#include "luvoasi/core/base.h"
#include "luvoasi/graphics/vertex_array.h"
#include "luvoasi/gui/font.h"

namespace Luvoasi {

/**
 * @brief 
 * @details
 * 
 */
class Renderer {
public:
    void BeginScene();
    void Submit(const VertexArray& va);
    void EndScene();
};

class FontRenderer {
public:
    FontRenderer();

    int DrawString(const std::wstring& string, float x, float y, float scale, float max_width);

    void Render();

    BitmapFont* GetBitmapFont() const;

private:
    FontLibrary m_font_lib;
    std::unique_ptr<FontFace> m_font_face;
    std::unique_ptr<BitmapFont> m_bitmap_font;

    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;

    std::unique_ptr<VertexArray> m_va;
};

}

#endif
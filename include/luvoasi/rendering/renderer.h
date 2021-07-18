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
 * Model-View-Viewmodel (MVVM)
 * 
 * Model = The scene which the renderer should render.
 * View = The renderer that traverses the scene and displays it.
 * View model = The data the renderer uses internally to render the scene.
 * 
 * Scene = Stores all the data required to render such as materials, meshes,
 *         instances of meshes, cameras, skeleton, instances of skeleton and etc.
 * Renderer = Reads the scene to render it.
 */
class Scene {
public:
    Scene();

private:

};

/**
 * @brief 
 * @details
 * 
 * - per framebuffers
 *   - per shaders
 *     - per textures (or material?)
 *       - per vertex arrays
 *         - draw an object (same model, but with different positions or scale)
 * 
 * some data => vertices, indices : constructing
 * vertices, indices => pixels : rendering or drawing?
 */
class Renderer {
public:
    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;

    virtual void Draw(const float* vertices, const unsigned int* indices) = 0;
    virtual void Draw(const VertexArray& va) = 0;
};

class Renderer2D : public Renderer {
public:
    void DrawQuad();
};

class FontRenderer {
public:
    FontRenderer();

    /**
     * @brief Draw given string origin to left-top corner of the screen.
     * 
     * @param string text to draw on the screen.
     * @param x in pixel units.
     * @param y in pixel units.
     * @param scale multiplicative factor of text. 1.0f by default.
     * @param max_width in pixel units. zero for infinite. 0 by default.
     * @return number of characters rendered on screen.
     */
    int DrawString(const std::wstring& string, float x, float y, float scale = 1.0f, float max_width = 0);

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
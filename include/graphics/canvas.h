#ifndef H_CANVAS
#define H_CANVAS

#include <vector>
#include <map>
#include <memory>

#include "graphics/mesh.h"
#include "graphics/font.h"
#include "graphics/texture.h"

/**
 * @class Canvas
 * @brief Provides batch rendering
 * 
 * `Canvas` class provides function for batch rendering.
 */
class Canvas {
public:
    Canvas();

    void Draw(Texture* texture, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void Draw(Texture* texture, MeshProfile& profile);
    void DrawLine();
    void DrawRect();

    FontRenderer& GetFontRenderer();

    void Render();
    void Clear();

private:
    std::unique_ptr<FontRenderer> font_renderer;

    std::unique_ptr<Mesh> mesh;
    std::unordered_map<Texture*, MeshProfile> mesh_profiles;
};

#endif
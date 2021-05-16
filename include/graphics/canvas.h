#ifndef H_CANVAS
#define H_CANVAS

#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/font.h"

/**
 * @class Canvas
 * @brief Provides batch rendering
 * 
 * `Canvas` class provides function for batch rendering.
 */
class Canvas {
public:
    Canvas();

    void Draw(Texture* texture, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw(Texture* texture, const MeshProfile& profile);
    void DrawLine(Texture* texture, const glm::vec3& a, const glm::vec3& b);
    void DrawRect(Texture* texture, const glm::vec3& pos, const glm::vec2& size);
    void DrawCircle(Texture* texture, const glm::vec3& pos, float radius);
    int DrawText(const glm::vec3& pos, const std::wstring& text, int max_width);

    void Render();
    void Clear();
    void Flush();

private:
    bool LoadGlyph(unsigned int codepoint);

    MeshProfile quad_profile;
    MeshProfile font_quad_profile;

    std::unique_ptr<Mesh> mesh;
    std::unordered_map<Texture*, MeshProfile> mesh_profiles;

    std::unique_ptr<FontLibrary> font_library;
    std::unique_ptr<FontFace> font_face;
    std::unordered_map<unsigned int, Glyph> glyph_infos;
    std::unique_ptr<TextureAtlas> texture_atlas;
};

#endif
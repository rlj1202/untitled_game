#include "graphics/canvas.h"

#include "graphics/mesh_builder.h"

Canvas::Canvas() {
    FontLibrary font_library;
    FontFace font_face = font_library.NewFontFace(
        "/res/D2Coding-Ver1.3.2-20180524-all.ttc"
        // "/res/NanumGothic.ttf"
    );
    font_renderer = std::make_unique<FontRenderer>(font_face);
}

void Canvas::Draw(Texture* texture, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    mesh_profiles[texture].Append(vertices, indices);
}

void Canvas::Draw(Texture* texture, MeshProfile& profile) {
    mesh_profiles[texture].Append(profile);
}

FontRenderer& Canvas::GetFontRenderer() {
    return *font_renderer;
}

void Canvas::Render() {
    for (auto key : mesh_profiles) {
        key.first->Bind();
        mesh = std::make_unique<Mesh>(BuildMesh(key.second));
        mesh->Draw();
    }

    font_renderer->Flush();
}

void Canvas::Clear() {
    for (auto& pair : mesh_profiles) {
        pair.second.Clear();
    }
}
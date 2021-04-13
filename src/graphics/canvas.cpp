#include "graphics/canvas.h"

#include "graphics/mesh_builder.h"

Canvas::Canvas() {
    FontLibrary font_library;
    FontFace font_face = font_library.NewFontFace(
        "/res/D2Coding-Ver1.3.2-20180524-all.ttc"
        // "/res/NanumGothic.ttf"
    );
    font_renderer = std::make_unique<FontRenderer>(font_face);

    vertices.reserve(sizeof(Vertex) * 100);
    indices.reserve(sizeof(unsigned int) * 100);
}

void Canvas::Draw(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());

    int cur_indices = this->indices.size();
    for (unsigned int index : indices) {
        this->indices.push_back(index + cur_indices);
    }
}

FontRenderer& Canvas::GetFontRenderer() {
    return *font_renderer;
}

void Canvas::Render() {
    mesh = std::make_unique<Mesh>(BuildMesh(vertices, indices));
    mesh->Draw();

    font_renderer->Flush();
}

void Canvas::Clear() {
    vertices.clear();
    indices.clear();
}
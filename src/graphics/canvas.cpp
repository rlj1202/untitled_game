#include "graphics/canvas.h"

#include "graphics/mesh_builder.h"

Canvas::Canvas() {
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

void Canvas::Render() {
    mesh = std::make_unique<Mesh>(BuildMesh(vertices, indices));
    mesh->Draw();
}

void Canvas::Clear() {
    vertices.clear();
    indices.clear();
}
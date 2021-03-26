#include "graphics/mesh.h"

#include <cstdio>

#include <glm/gtx/transform.hpp>

#define DEBUG
#include "debug.h"

Vertex::Vertex(glm::vec3 position, glm::vec2 tex_coord)
    : position(position), tex_coord(tex_coord) {
}

MeshProfile::MeshProfile()
    : vertices(std::vector<Vertex>()), indices(std::vector<unsigned int>()) {
}

MeshProfile::MeshProfile(const MeshProfile &o)
    : vertices(o.vertices), indices(o.indices) {
}

MeshProfile::MeshProfile(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string texture_name)
    : vertices(vertices), indices(indices), texture_name(texture_name) {
}

MeshProfile MeshProfile::translate(glm::vec3 v) {
    MeshProfile new_profile(*this);

    for (Vertex &vertex : new_profile.vertices) {
        vertex.position += v;
    }

    return new_profile;
}

MeshProfile MeshProfile::scale(glm::vec3 v) {
    MeshProfile new_profile(*this);

    for (Vertex &vertex : new_profile.vertices) {
        vertex.position *= v;
    }

    return new_profile;
}

MeshProfile MeshProfile::rotate(float angle, glm::vec3 dir) {
    MeshProfile new_profile(*this);

    glm::mat4 mat = glm::rotate(angle, dir);

    for (Vertex &vertex : new_profile.vertices) {
        vertex.position = (mat * glm::vec4(vertex.position, 1));
    }

    return new_profile;
}

MeshProfile& MeshProfile::append(const MeshProfile &o) {
    int cur_vertices = vertices.size();
    
    vertices.insert(vertices.end(), o.vertices.begin(), o.vertices.end());

    for (int index : o.indices) {
        indices.push_back(index + cur_vertices);
    }

    return *this;
}

Mesh::Mesh(unsigned int vao_id, std::vector<unsigned int> vbo_id, unsigned int ebo_id, unsigned int cnt_vertices)
    : vao_id(vao_id), vbo_id(vbo_id), ebo_id(ebo_id), cnt_vertices(cnt_vertices) {
    DEBUG_STDOUT("mesh created : %d\n", vao_id);
}

Mesh::Mesh(Mesh&& mesh) {
    vao_id = mesh.vao_id;
    vbo_id = mesh.vbo_id;
    ebo_id = mesh.ebo_id;
    cnt_vertices = mesh.cnt_vertices;

    mesh.vao_id = 0;
    mesh.vbo_id.clear();
    mesh.ebo_id = 0;
}

Mesh::~Mesh() {
    if (vao_id) DEBUG_STDOUT("mesh deleted : %d\n", vao_id);

    if (vao_id) glDeleteVertexArrays(1, &vao_id);
    if (!vbo_id.empty()) glDeleteBuffers(vbo_id.size(), &vbo_id[0]);
    if (ebo_id) glDeleteBuffers(1, &ebo_id);
}

void Mesh::Bind() {
    glBindVertexArray(vao_id);
}

void Mesh::Draw() {
    glDrawElements(GL_TRIANGLES, cnt_vertices, GL_UNSIGNED_INT, 0);
}
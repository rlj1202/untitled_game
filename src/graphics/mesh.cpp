#include "graphics/mesh.h"

#include <cstdio>

#include <glm/gtx/transform.hpp>

#define DEBUG
#include "debug.h"

Vertex::Vertex() {

}

Vertex::Vertex(glm::vec3 position, glm::vec2 tex_coord)
    : position(position), tex_coord(tex_coord) {
}

MeshProfile::MeshProfile()
    : vertices(std::vector<Vertex>()), indices(std::vector<unsigned int>()) {
}

MeshProfile::MeshProfile(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, ITexture* texture)
    : vertices(vertices), indices(indices), texture(texture) {
}

MeshProfile MeshProfile::Clone() const {
    return *this;
}

MeshProfile& MeshProfile::TexTranslate(const glm::vec2& v) {
    for (Vertex& vertex : vertices) {
        vertex.tex_coord += v;
    }

    return *this;
}

MeshProfile& MeshProfile::TexScale(const glm::vec2& v) {
    for (Vertex& vertex : vertices) {
        vertex.tex_coord *= v;
    }

    return *this;
}

MeshProfile& MeshProfile::TexMul(const glm::mat4& mat) {
    for (Vertex& vertex : vertices) {
        vertex.tex_coord = (mat * glm::vec4(vertex.tex_coord, 0, 1));
    }

    return *this;
}

MeshProfile& MeshProfile::Translate(const glm::vec3& v) {
    for (Vertex& vertex : vertices) {
        vertex.position += v;
    }

    return *this;
}

MeshProfile& MeshProfile::Scale(const glm::vec3& v) {
    for (Vertex& vertex : vertices) {
        vertex.position *= v;
    }

    return *this;
}

MeshProfile& MeshProfile::Rotate(float angle, glm::vec3 dir) {
    glm::mat4 mat = glm::rotate(angle, dir);

    for (Vertex& vertex : vertices) {
        vertex.position = (mat * glm::vec4(vertex.position, 1));
    }

    return *this;
}

MeshProfile& MeshProfile::Mul(const glm::mat4& mat) {
    for (Vertex& vertex : vertices) {
        vertex.position = (mat * glm::vec4(vertex.position, 1));
    }

    return *this;
}

MeshProfile& MeshProfile::SetTexture(ITexture* texture) {
    this->texture = texture;

    return *this;
}

MeshProfile& MeshProfile::Append(const MeshProfile &o) {
    int cur_vertices = vertices.size();
    
    vertices.insert(vertices.end(), o.vertices.begin(), o.vertices.end());

    for (int index : o.indices) {
        indices.emplace_back(index + cur_vertices);
        // indices.push_back(index + cur_vertices);
    }

    return *this;
}

MeshProfile& MeshProfile::Append(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    int cur_vertices = this->vertices.size();
    
    this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());

    for (int index : indices) {
        this->indices.emplace_back(index + cur_vertices);
        // this->indices.push_back(index + cur_vertices);
    }

    return *this;
}

MeshProfile& MeshProfile::Clear() {
    vertices.clear();
    indices.clear();
    vertices.reserve(16);
    indices.reserve(16);

    return *this;
}

Mesh::Mesh(unsigned int vao_id, std::vector<unsigned int> vbo_id, unsigned int ebo_id, unsigned int cnt_vertices)
    : vao_id(vao_id), vbo_id(vbo_id), ebo_id(ebo_id), cnt_vertices(cnt_vertices) {
    // DEBUG_STDOUT("mesh created : %d\n", vao_id);
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
    // if (vao_id) DEBUG_STDOUT("mesh deleted : %d\n", vao_id);

    if (vao_id) glDeleteVertexArrays(1, &vao_id);
    if (!vbo_id.empty()) glDeleteBuffers(vbo_id.size(), &vbo_id[0]);
    if (ebo_id) glDeleteBuffers(1, &ebo_id);
}

void Mesh::Bake() {
    // do nothing
}

void Mesh::Bind() {
    glBindVertexArray(vao_id);
}

void Mesh::Unbind() {
    glBindVertexArray(0);
}

void Mesh::Draw() {
    Bind();
    glDrawElements(GL_TRIANGLES, cnt_vertices, GL_UNSIGNED_INT, 0);
}

// temporary namespace
namespace _dev {

BufferElement::BufferElement(int index, int size, unsigned int gl_type, int bytes_of_type)
    : index(index), size(size), gl_type(gl_type), bytes_of_type(bytes_of_type) {

}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
    : elements(elements) {
    
}

std::vector<BufferElement>& BufferLayout::GetElements() {
    return elements;
}

Mesh::Mesh(std::vector<ITexture*> textures) : vao_id(0), textures(textures) {
    indices_buffer = std::make_unique<Buffer<unsigned int>>(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW
    );
}

Mesh::Mesh(Mesh&& o)
    : buffers(std::move(o.buffers)), indices_buffer(std::move(o.indices_buffer)) {
    vao_id = o.vao_id;
    o.vao_id = 0;
}

Mesh::~Mesh() {
    if (vao_id) {
        glDeleteVertexArrays(1, &vao_id);
        vao_id = 0;
    }
}

Mesh& Mesh::operator=(Mesh&& o) noexcept {
    if (this == &o) return *this;

    // Delete existing vao if it is.
    if (vao_id) {
        glDeleteVertexArrays(1, &vao_id);
    }
    vao_id = o.vao_id;
    o.vao_id = 0;

    buffers = std::move(o.buffers);
    indices_buffer = std::move(o.indices_buffer);

    return *this;
}

void Mesh::AttachBuffer(std::unique_ptr<IBuffer> buffer, BufferLayout layout) {
    Bind();
    buffer->Bind();

    uint32_t stride = 0;
    for (auto& element : layout.GetElements()) {
        stride += element.size * element.bytes_of_type;
    }

    intptr_t offset = 0;
    for (auto& element : layout.GetElements()) {
        glVertexAttribPointer(element.index, element.size, element.gl_type, GL_FALSE, stride, (void*) offset);
        glEnableVertexAttribArray(element.index);

        offset += element.size * element.bytes_of_type;
    }

    Unbind();

    buffers.push_back(std::move(buffer));
}

void Mesh::AttachElementArrayBuffer(std::unique_ptr<Buffer<unsigned int>> buffer) {
    Bind();
    buffer->Bind();

    Unbind();

    this->indices_buffer = std::move(buffer);
}

void Mesh::Bake() {
    if (vao_id == 0) {
        glGenVertexArrays(1, &vao_id);
    }
    glBindVertexArray(vao_id);

    for (auto& buffer : buffers) {
        buffer->Bake();
    }

    indices_buffer->Bake();

    glBindVertexArray(0);
}

void Mesh::Bind() {
    if (vao_id == 0) {
        Bake();
    }

    glBindVertexArray(vao_id);
}

void Mesh::Unbind() {
    glBindVertexArray(0);
}

void Mesh::Draw() {
    if (vao_id) {
        for (int i = 0; i < textures.size(); i++) {
            textures[i]->Bind(i);
        }
        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, indices_buffer->Size(), GL_UNSIGNED_INT, (void*) 0);
    }
}

// end of temporary namespace
}
#ifndef H_MESH
#define H_MESH

#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>

class Vertex {
public:
    Vertex(glm::vec3 position, glm::vec2 tex_coord);
    glm::vec3 position;
    glm::vec2 tex_coord;
};

class MeshProfile {
public:
    MeshProfile();
    MeshProfile(const MeshProfile &o);
    MeshProfile(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string texture_name);

    MeshProfile Translate(glm::vec3 v);
    MeshProfile Scale(glm::vec3 v);
    MeshProfile Rotate(float angle, glm::vec3 dir);
    MeshProfile& Append(const MeshProfile &o);

    MeshProfile TexTranslate(glm::vec2 v);
    MeshProfile TexScale(glm::vec2 v);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::string texture_name;
};

class Mesh {
public:
    Mesh(unsigned int vao_id, std::vector<unsigned int> vbo_id, unsigned int ebo_id, unsigned int cnt_vertices);
    Mesh(const Mesh &o) = delete;
    Mesh(Mesh&& mesh);
    ~Mesh();

    void Bind();
    void Draw();

    Mesh& operator=(const Mesh &o) = delete;

private:
    unsigned int vao_id;
    std::vector<unsigned int> vbo_id;
    unsigned int ebo_id;
    unsigned int cnt_vertices;
};

#endif
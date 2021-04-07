#ifndef H_MESH
#define H_MESH

#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>

#define DEBUG
#include "debug.h"

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






template<typename Datatype, int... Ptrs>
class Vbo {
};

template <typename... Vbos>
class _Mesh {
private:
    template<typename DataType>
    class GLType {
    public:
        static const unsigned int value;
    };

    template<>
    class GLType<float> { public: static const unsigned int value = GL_FLOAT; };
    template<>
    class GLType<int> { public: static const unsigned int value = GL_INT; };

    template<typename... Ts>
    class VboIter {
    public:
        void CreateVbos(int loc);
        void DeleteVbos();
    };

    template<>
    class VboIter<> {
    public:
        void CreateVbos(int loc) {}
        void DeleteVbos() {}
    };

    template<typename DataType, int... Ptrs, typename... Rests>
    class VboIter<Vbo<DataType, Ptrs...>, Rests...> : public VboIter<Rests...> {
    public:
        template<typename Type, typename... Types>
        void CreateVbos(int loc, Type data, Types... rests) {
            glGenBuffers(1, &vbo_id);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBufferData(GL_ARRAY_BUFFER, sizeof(DataType) * data.size(), (void*) &data[0], GL_STATIC_DRAW);

            IteratePtrs(loc, 0, Ptrs...);

            VboIter<Rests...>::CreateVbos(loc + sizeof...(Ptrs), rests...);
        }

        void DeleteVbos() {
            glDeleteBuffers(1, &vbo_id);

            VboIter<Rests...>::DeleteVbos();
        }

        void CreatePtr(int loc, int offset, int size) {
            unsigned int type = GLType<DataType>::value;
            int total = (Ptrs + ...);

            glVertexAttribPointer(
                loc, size, type, GL_FALSE,
                sizeof(DataType) * total, (void*) (sizeof(DataType) * offset)
            );
            glEnableVertexAttribArray(loc);

            DEBUG_STDOUT("Set vertex pointer : loc = %d, offset = %d, size = %d\n", loc, offset, size);
        }

        void IteratePtrs(int loc, int offset, int size) {
            CreatePtr(loc, offset, size);
        }

        void IteratePtrs(int loc, int offset, int size, int rests...) {
            CreatePtr(loc, offset, size);

            IteratePtrs(loc + 1, offset + size, rests);
        }

        unsigned int vbo_id;
    };

public:
    template<typename... Types>
    _Mesh(std::vector<unsigned int> indices, Types... data) {
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        vbo_iter.CreateVbos(0, data...);

        glGenBuffers(1, &ebo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), (void*) &indices[0], GL_STATIC_DRAW);

        cnt_vertices = indices.size();

        glBindVertexArray(0);
    }
    ~_Mesh() {
        if (vao_id) {
            glDeleteVertexArrays(1, &vao_id);
        }
        vbo_iter.DeleteVbos();
        if (ebo_id) {
            glDeleteBuffers(1, &ebo_id);
        }
    }

    void Bind() {
        glBindVertexArray(vao_id);
    }

    void Draw() {
        glDrawElements(GL_TRIANGLES, cnt_vertices, GL_UNSIGNED_INT, (void*) 0);
    }

private:
    unsigned int vao_id;
    unsigned int ebo_id;
    unsigned int cnt_vertices;

    VboIter<Vbos...> vbo_iter;
};

#endif
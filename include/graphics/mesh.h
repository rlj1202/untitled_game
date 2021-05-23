#ifndef H_MESH
#define H_MESH

#include <string>
#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>

#include "graphics/texture.h"

#define DEBUG
#include "debug.h"

/**
 * @brief Minimum unit of drawable thing. Contains information of primitives and textures.
 * @details
 * Mesh class contains minimal and complete informations to draw thing in scene.
 * Primitives, which have coordinates and tex coordinates and textures, such as diffuse or
 * normal map, etc.
 * 
 * Single mesh object can be drawn alone but it can be batched as one to draw together which
 * have same textures.
 */
class IMesh {
public:
    virtual ~IMesh() {}

    virtual void Bake() = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Draw() = 0;
};

class Vertex {
public:
    Vertex();
    Vertex(glm::vec3 position, glm::vec2 tex_coord);
    
    glm::vec3 position;
    glm::vec2 tex_coord;
};

class MeshProfile {
public:
    MeshProfile();
    MeshProfile(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, ITexture* texture);

    MeshProfile Clone() const;

    MeshProfile& Translate(const glm::vec3& v);
    MeshProfile& Scale(const glm::vec3& v);
    MeshProfile& Rotate(float angle, glm::vec3 dir);

    MeshProfile& Mul(const glm::mat4& mat);

    MeshProfile& TexTranslate(const glm::vec2& v);
    MeshProfile& TexScale(const glm::vec2& v);

    MeshProfile& TexMul(const glm::mat4& mat);

    MeshProfile& SetTexture(ITexture* texture);

    MeshProfile& Append(const MeshProfile &o);
    MeshProfile& Append(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    MeshProfile& Clear();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    ITexture* texture;
};

class Mesh : public IMesh {
public:
    Mesh(unsigned int vao_id, std::vector<unsigned int> vbo_id, unsigned int ebo_id, unsigned int cnt_vertices);
    Mesh(const Mesh &o) = delete;
    Mesh(Mesh&& mesh);
    ~Mesh();

    void Bake() override;
    void Bind() override;
    void Unbind() override;
    void Draw() override;

    Mesh& operator=(const Mesh &o) = delete;

private:
    unsigned int vao_id;
    std::vector<unsigned int> vbo_id;
    unsigned int ebo_id;
    unsigned int cnt_vertices;
};

class IBuffer {
public:
    virtual ~IBuffer() {}

    /**
     * Number of elements in buffer.
     */
    virtual unsigned int Size() = 0;

    /**
     * Size of single element in buffer in bytes.
     */
    virtual unsigned int Stride() = 0;

    virtual void Bake() = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
};

template<typename T>
class Buffer : public IBuffer {
public:
    Buffer(unsigned int target, unsigned int usage);
    Buffer(const Buffer& o);
    Buffer(Buffer&& o);
    ~Buffer();

    Buffer& operator=(const Buffer& o);
    Buffer& operator=(Buffer&& o) noexcept;

    void SetData(unsigned int length, const T* data);
    void SetData(std::vector<T>& data);

    unsigned int Size() override;
    unsigned int Stride() override;

    void Bake() override;
    void Bind() override;
    void Unbind() override;

private:
    unsigned int target;
    unsigned int usage;

    unsigned int buffer_id;

    std::vector<T> data;
};

// TODO: 
template<typename T>
class SubBuffer : public IBuffer {
public:
    SubBuffer(IBuffer* buffer, unsigned int offset);

    void SetData(unsigned int length, const T* data);
    void SetData(std::vector<T>& data);

    unsigned int Size() override;
    unsigned int Stride() override;

    void Bake() override;
    void Bind() override;
    void Unbind() override;

private:
    IBuffer* buffer;

    /// Offset in buffer in bytes.
    unsigned int offset;
};

#include "graphics/mesh.tcc"

// temporary namespace
namespace _dev {

class BufferElement {
public:
    BufferElement(int index, int size, unsigned int gl_type, int bytes_of_type);

    int index;
    int size;
    unsigned int gl_type;
    int bytes_of_type;
};

class BufferLayout {
public:
    BufferLayout(std::initializer_list<BufferElement> elements);

    std::vector<BufferElement>& GetElements();

private:
    std::vector<BufferElement> elements;
};

/**
 * 
 */
class Mesh : public IMesh {
public:
    Mesh(std::vector<ITexture*> textures);
    Mesh(const Mesh& o) = delete;
    Mesh(Mesh&& o);
    ~Mesh();

    Mesh& operator=(const Mesh& o) = delete;
    Mesh& operator=(Mesh&& o) noexcept;

    void AttachBuffer(std::unique_ptr<IBuffer> buffer, BufferLayout layout);
    void AttachElementArrayBuffer(std::unique_ptr<Buffer<unsigned int>> buffer);

    void Bake() override;
    void Bind() override;
    void Unbind() override;
    void Draw() override;

private:
    unsigned int vao_id;

    std::vector<std::unique_ptr<IBuffer>> buffers;
    std::unique_ptr<Buffer<unsigned int>> indices_buffer;

    std::vector<ITexture*> textures;
};

// end of temporary namespace
}

/*
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
*/

/*
    {
        std::vector<float> vertices = {
            -1.0f, 0.0f, 0.0f, 
             1.0f, 0.0f, 0.0f, 
             0.0f, 1.0f, 0.0f, 
        };
        std::vector<float> tex_coords = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.5f, 0.0f,
        };
        std::vector<float> something_else = {
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
        };

        test_mesh = std::make_unique<
            _Mesh<
                Vbo<float, 3>,
                Vbo<float, 2>,
                Vbo<float, 3, 3>
            >
        >(indices, vertices, tex_coords, something_else);
    }
 */

#endif
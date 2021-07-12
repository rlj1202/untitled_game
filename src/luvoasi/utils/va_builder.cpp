#include <vector>

#include <glm/glm.hpp>

#include "luvoasi/core/base.h"
#include "luvoasi/graphics/vertex_array.h"

namespace Luvoasi {

class VABuilder {
public:
    void AddRect(glm::vec2 lb, glm::vec2 ru);

    std::unique_ptr<VertexArray> Bake() const;

private:
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
};

void VABuilder::AddRect(glm::vec2 lb, glm::vec2 ru) {
    unsigned int last = m_vertices.size();

    m_vertices.reserve(m_vertices.size() + 4);
    m_vertices.insert(m_vertices.end(), { lb.x, lb.y, 0.0f, 0.0f, 1.0f });
    m_vertices.insert(m_vertices.end(), { ru.x, lb.y, 0.0f, 1.0f, 1.0f });
    m_vertices.insert(m_vertices.end(), { ru.x, ru.y, 0.0f, 1.0f, 0.0f });
    m_vertices.insert(m_vertices.end(), { lb.x, ru.y, 0.0f, 0.0f, 0.0f });

    m_indices.reserve(m_indices.size() + 6);
    m_indices.insert(m_indices.end(), {
        last + 0, last + 1, last + 2,
        last + 0, last + 2, last + 3,
    });
}

std::unique_ptr<VertexArray> VABuilder::Bake() const {
    BufferLayout layout = {
        BufferElement{ 0, 3, false, DataType::FLOAT32 },
        BufferElement{ 1, 2, false, DataType::FLOAT32 },
    };

    auto array_buffer = ArrayBuffer::CreateBuffer(m_vertices.data(), m_vertices.size());
    auto index_buffer = IndexBuffer::CreateBuffer(m_indices.data(), m_indices.size());

    auto va = VertexArray::CreateVertexArray();
    va->AttachArrayBuffer(std::move(array_buffer), layout);
    va->AttachIndexBuffer(std::move(index_buffer));

    return std::move(va);
}

}
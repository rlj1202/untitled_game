#include "platforms/luvoasi/graphics/vertex_array_opengl.h"

#include <GLES3/gl3.h>

namespace Luvoasi {

OpenGLVertexArray::OpenGLVertexArray() {
    glGenVertexArrays(1, &m_id);

    glBindVertexArray(m_id);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    if (m_id) {
        glDeleteVertexArrays(1, &m_id);
        m_id = 0;
    }
}

uint32_t OpenGLVertexArray::GetId() const {
    return m_id;
}

void OpenGLVertexArray::Bind() const {
    glBindVertexArray(m_id);
}

void OpenGLVertexArray::Unbind() const {
    glBindVertexArray(0);
}

}

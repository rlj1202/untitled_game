#include "platforms/luvoasi/graphics/buffer_opengl.h"

#include <GLES3/gl3.h>

namespace Luvoasi {

OpenGLBuffer::OpenGLBuffer() {
    glGenBuffers(1, &m_id);
}

OpenGLBuffer::~OpenGLBuffer() {
    if (m_id) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }
}

uint32_t OpenGLBuffer::GetId() const {
    return m_id;
}

void OpenGLBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void OpenGLBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}

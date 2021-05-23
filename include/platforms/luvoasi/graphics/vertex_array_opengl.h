#ifndef H_LUVOASI_VERTEX_ARRAY_OPENGL
#define H_LUVOASI_VERTEX_ARRAY_OPENGL

#include "luvoasi/graphics/vertex_array.h"

namespace Luvoasi {

class OpenGLVertexArray : public VertexArray {
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    uint32_t GetId() const override;

    void Bind() const override;
    void Unbind() const override;

private:
    uint32_t m_id;
};

}

#endif
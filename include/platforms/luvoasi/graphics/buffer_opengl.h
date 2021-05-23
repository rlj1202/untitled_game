#ifndef H_LUVOASI_BUFFER_OPENGL
#define H_LUVOASI_BUFFER_OPENGL

#include "luvoasi/graphics/buffer.h"

namespace Luvoasi {

class OpenGLBuffer : public Buffer {
public:
    OpenGLBuffer();
    virtual ~OpenGLBuffer();

    uint32_t GetId() const override;



    void Bind() const override;
    void Unbind() const override;
private:
    uint32_t m_id;
};

}

#endif
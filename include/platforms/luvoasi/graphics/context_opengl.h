#ifndef H_LUVOASI_CONTEXT_OPENGL
#define H_LUVOASI_CONTEXT_OPENGL

#include "luvoasi/graphics/context.h"

namespace Luvoasi {

class OpenGLContext : public Context {
public:
    virtual ~OpenGLContext();

    void Init() override;

    void SwapBuffers() override;
    void PollEvents() override;

    void Clear() override;
    void ClearColor(float r, float g, float b, float a) override;
};

}

#endif
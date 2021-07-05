#ifndef H_LUVOASI_CONTEXT
#define H_LUVOASI_CONTEXT

#include "luvoasi/core/base.h"

namespace Luvoasi {

class Context {
public:
    virtual ~Context() = default;

    virtual void Init() = 0;

    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;

    virtual void Clear() = 0;
    virtual void ClearColor(float r, float g, float b, float a) = 0;
};

}

#endif
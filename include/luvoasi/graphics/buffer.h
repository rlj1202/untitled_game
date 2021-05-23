#ifndef H_LUVOASI_BUFFER
#define H_LUVOASI_BUFFER

#include "luvoasi/core/base.h"

namespace Luvoasi {

class BufferElement {
public:

};

class BufferLayout {
public:

};

class Buffer {
public:
    virtual ~Buffer() = default;

    virtual uint32_t GetId() const = 0;

    virtual void SetData(void* data, uint32_t size) = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
};

}

#endif
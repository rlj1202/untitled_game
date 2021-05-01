#include "graphics/mesh.h"

#define DEBUG
#include "debug.h"

template<typename T>
Buffer<T>::Buffer(unsigned int target, unsigned int usage)
    : target(target), usage(usage), buffer_id(0) {

    DEBUG_STDOUT("Buffer<T>::constructor : %d\n", buffer_id);
}

template<typename T>
Buffer<T>::Buffer(const Buffer& o)
    : target(o.target), usage(o.usage), data(o.data) {

    DEBUG_STDOUT("Buffer<T>::copy constructor : %d\n", o.buffer_id);
}

template<typename T>
Buffer<T>::Buffer(Buffer<T> &&o) {
    target = o.target;
    usage = o.usage;

    buffer_id = o.buffer_id;
    o.buffer_id = 0;

    data = std::move(o.data);

    DEBUG_STDOUT("Buffer<T>::move constructor : %d\n", buffer_id);
}

template<typename T>
Buffer<T>::~Buffer() {
    if (buffer_id) {
        DEBUG_STDOUT("Buffer<T>::~Buffer() : %d\n", buffer_id);

        glDeleteBuffers(1, &buffer_id);
        buffer_id = 0;
    }
}

template<typename T>
Buffer<T>& Buffer<T>::operator=(const Buffer<T>& o) {
    DEBUG_STDOUT("Buffer<T>::copy assignment : %d\n", o.buffer_id);

    if (this == &o) return *this;

    target = o.target;
    usage = o.usage;

    data = o.data;

    return *this;
}

template<typename T>
Buffer<T>& Buffer<T>::operator=(Buffer<T>&& o) noexcept {
    DEBUG_STDOUT("Buffer<T>::move assignment : %d %d\n", buffer_id, o.buffer_id);

    if (this == &o) return *this;

    target = o.target;
    usage = o.usage;

    // Delete existing buffer if it is
    if (buffer_id) {
        glDeleteBuffers(1, &buffer_id);
    }
    buffer_id = o.buffer_id;
    o.buffer_id = 0;

    data = std::move(o.data);

    return *this;
}

template<typename T>
void Buffer<T>::SetData(unsigned int length, const T* data) {
    this->data.assign(data, data + sizeof(T) * length);
}

template<typename T>
void Buffer<T>::SetData(std::vector<T>& data) {
    this->data.assign(data.begin(), data.end());
}

template<typename T>
unsigned int Buffer<T>::Size() {
    return (unsigned int) data.size();
}

template<typename T>
unsigned int Buffer<T>::Stride() {
    return sizeof(T);
}

template<typename T>
void Buffer<T>::Bake() {
    if (buffer_id == 0) {
        glGenBuffers(1, &buffer_id);
    }
    glBindBuffer(target, buffer_id);
    glBufferData(target, sizeof(T) * data.size(), (const void*) data.data(), usage);
}

template<typename T>
void Buffer<T>::Bind() {
    if (buffer_id == 0) {
        Bake();
    }
    
    glBindBuffer(target, buffer_id);
}

template<typename T>
void Buffer<T>::Unbind() {
    glBindBuffer(target, 0);
}
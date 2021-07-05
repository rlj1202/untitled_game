/**
 * @file buffer.h
 * @author Jisu Sim (rlj1202@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef H_LUVOASI_BUFFER
#define H_LUVOASI_BUFFER

#include <utility>
#include <vector>
#include <memory>

#include "luvoasi/core/base.h"
#include "luvoasi/core/types.h"

namespace Luvoasi {

/**
 * @brief Describes a component in an array buffer.
 * @details 
 * 
 */
struct BufferElement {
    uint32_t index;

    /// must be between 1 ~ 4, the number of components.
    uint32_t size;

    /// if this option is set to true, a component will be normalized to have
    ///  length 1 before sent to shader program.
    bool normalize;

    DataType::Value internal_type;
};

/**
 * @brief 
 * @details
 * 
 */
class BufferLayout {
public:
    BufferLayout();
    BufferLayout(std::initializer_list<BufferElement> elements);

    std::vector<BufferElement>& GetElements();
    const std::vector<BufferElement>& GetElements() const;

private:
    std::vector<BufferElement> m_elements;
};

enum class BufferType {
    ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER,
};

enum class BufferUsage {
    /// data will be modified once and used many times.
    STATIC_DRAW,
    /// data will be modified repeatedly and used many times.
    DYNAMIC_DRAW,
    /// data will be modified once and used at most a few times.
    /// in some platforms, STREAM_DRAW and DYNAMIC_DRAW are equivalent.
    STREAM_DRAW,
};

/**
 * @brief 
 * @details
 * 
 * @tparam T BufferType
 * @tparam U internal element data type
 */
template<BufferType T, typename U>
class Buffer {
public:
    virtual ~Buffer() = default;

    virtual uint32_t GetId() const = 0;

    virtual void SetData(const U* data, uint32_t size) = 0;

    virtual uint32_t GetLength() const = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    static std::unique_ptr<Buffer<T, U>> CreateBuffer(const U* data, uint32_t size);
};

using ArrayBuffer = Buffer<BufferType::ARRAY_BUFFER, float>;
using IndexBuffer = Buffer<BufferType::ELEMENT_ARRAY_BUFFER, unsigned int>;

}

#endif

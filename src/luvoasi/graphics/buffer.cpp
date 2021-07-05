#include "luvoasi/graphics/buffer.h"

namespace Luvoasi {

/*******************************************************************************
 * BufferLayout
 ******************************************************************************/

BufferLayout::BufferLayout() {
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
    : m_elements(elements) {
}

/*******************************************************************************
 * BufferElement
 ******************************************************************************/

std::vector<BufferElement>& BufferLayout::GetElements() {
    return m_elements;
}

const std::vector<BufferElement>& BufferLayout::GetElements() const {
    return m_elements;
}

}

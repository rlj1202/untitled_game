#include "luvoasi/graphics/shader.h"

namespace Luvoasi {

template<typename T>
void ShaderProgram::SetUniform(const char* name, const T& value) {
    SetUniform(GetUniformLocation(name), value);
}

}
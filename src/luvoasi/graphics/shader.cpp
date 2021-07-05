#include "luvoasi/graphics/shader.h"

#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Luvoasi {

template<>
void ShaderProgram::SetUniform(int32_t loc, const bool& value) {
    SetUniform1i(loc, (int) value);
}

template<>
void ShaderProgram::SetUniform(int32_t loc, const int& value) {
    SetUniform1i(loc, value);
}

template<>
void ShaderProgram::SetUniform(int32_t loc, const glm::vec2& value) {
    SetUniform2i(loc, value.x, value.y);
}

template<>
void ShaderProgram::SetUniform(int32_t loc, const glm::vec3& value) {
    SetUniform3i(loc, value.x, value.y, value.z);
}

template<>
void ShaderProgram::SetUniform(int32_t loc, const glm::vec4& value) {
    SetUniform4i(loc, value.x, value.y, value.z, value.w);
}

template<>
void ShaderProgram::SetUniform(int32_t loc, const glm::mat4& value) {
    SetUniformMat4fv(loc, 1, false, glm::value_ptr(value));
}

std::unique_ptr<Shader> Shader::CreateShaderFromFile(ShaderType type, const char* path) {
    std::ifstream file(path);
    std::stringstream str_stream;
    
    str_stream << file.rdbuf();

    file.close();

    return std::move(CreateShader(type, str_stream.str().c_str()));
}

std::unique_ptr<ShaderProgram> ShaderProgram::CreateShaderProgramFromFiles(const char* vertex_shader_path, const char* fragment_shader_path) {
    auto shader_vert = Shader::CreateShaderFromFile(
        ShaderType::VERTEX_SHADER, vertex_shader_path
    );
    auto shader_frag = Shader::CreateShaderFromFile(
        ShaderType::FRAGMENT_SHADER, fragment_shader_path
    );

    return std::move(CreateShaderProgram({shader_vert.get(), shader_frag.get()}));
}

}
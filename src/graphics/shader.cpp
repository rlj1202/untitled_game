#include "graphics/shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEBUG
#include "debug.h"

unsigned int CompileShader(GLenum type, const char* code) {
    unsigned int shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &code, nullptr);
    glCompileShader(shader_id);

    int success;
    char info_log[512];
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_id, sizeof(info_log), nullptr, info_log);
        DEBUG_STDOUT("%s\n", info_log);
    }

    return shader_id;
}

Shader::Shader(const char* vertex_shader_code, const char* fragment_shader_code) {
    unsigned int vertex_shader_id = CompileShader(GL_VERTEX_SHADER, vertex_shader_code);
    unsigned int fragment_shader_id = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    int success;
    char info_log[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
}

Shader::Shader(Shader&& o) {
    program_id = o.program_id;
    o.program_id = 0;
}

Shader::~Shader() {
    if (program_id) glDeleteProgram(program_id);
}

Shader& Shader::operator=(Shader&& o) noexcept {
    if (this == &o) return *this;

    if (program_id) {
        glDeleteProgram(program_id);
    }
    program_id = o.program_id;
    o.program_id = 0;

    return *this;
}

void Shader::Use() {
    glUseProgram(program_id);
}

template<>
void Shader::SetUniform<bool>(const std::string& name, bool value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform1i(loc, (int) value);
}

template<>
void Shader::SetUniform<int>(const std::string& name, int value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform1i(loc, value);
}

template<>
void Shader::SetUniform<float>(const std::string& name, float value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform1f(loc, value);
}

template<>
void Shader::SetUniform<glm::vec4>(const std::string& name, glm::vec4 value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

template<>
void Shader::SetUniform<glm::mat4>(const std::string& name, glm::mat4 value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
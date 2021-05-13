#include "graphics/shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEBUG
#include "debug.h"

Shader::Shader(GLenum type, const char* code) {
    id = glCreateShader(type);
    glShaderSource(id, 1, &code, nullptr);
    glCompileShader(id);

    int success;
    char info_log[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, sizeof(info_log), nullptr, info_log);
        DEBUG_STDOUT("%s\n", info_log);
    }
}

Shader::Shader(Shader&& o) {
    id = o.id;
    o.id = 0;
}

Shader::~Shader() {
    if (id) {
        glDeleteShader(id);
        id = 0;
    }
}

Shader& Shader::operator=(Shader&& o) noexcept {
    id = o.id;
    o.id = 0;

    return *this;
}

unsigned int Shader::GetId() const {
    return id;
}

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders) {
    program_id = glCreateProgram();

    for (const Shader& shader : shaders) {
        glAttachShader(program_id, shader.GetId());
    }
    glLinkProgram(program_id);

    int success;
    char info_log[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);
    }
}

ShaderProgram::ShaderProgram(const char* vertex_shader_code, const char* fragment_shader_code) {
    Shader vertex_shader(GL_VERTEX_SHADER, vertex_shader_code);
    Shader fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader.GetId());
    glAttachShader(program_id, fragment_shader.GetId());
    glLinkProgram(program_id);

    int success;
    char info_log[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& o) {
    program_id = o.program_id;
    o.program_id = 0;
}

ShaderProgram::~ShaderProgram() {
    if (program_id) glDeleteProgram(program_id);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& o) noexcept {
    if (this == &o) return *this;

    if (program_id) {
        glDeleteProgram(program_id);
    }
    program_id = o.program_id;
    o.program_id = 0;

    return *this;
}

void ShaderProgram::Use() {
    glUseProgram(program_id);
}

template<>
void ShaderProgram::SetUniform<bool>(const std::string& name, bool value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform1i(loc, (int) value);
}

template<>
void ShaderProgram::SetUniform<int>(const std::string& name, int value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform1i(loc, value);
}

template<>
void ShaderProgram::SetUniform<float>(const std::string& name, float value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform1f(loc, value);
}

template<>
void ShaderProgram::SetUniform<glm::vec4>(const std::string& name, glm::vec4 value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

template<>
void ShaderProgram::SetUniform<glm::mat4>(const std::string& name, glm::mat4 value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
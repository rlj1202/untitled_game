#include "graphics/shader.h"

#define DEBUG
#include "debug.h"

unsigned int CompileShader(GLenum type, const char *code) {
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

Shader::Shader(const char *vertex_shader_code, const char *fragment_shader_code) {
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

Shader::Shader(Shader &&o) {
    program_id = o.program_id;
    o.program_id = 0;
}

Shader::~Shader() {
    if (program_id) glDeleteProgram(program_id);
}

void Shader::Use() {
    glUseProgram(program_id);
}

void Shader::SetUniformBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int) value);
}

void Shader::SetUniformInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

void Shader::SetUniformFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}

void Shader::SetUniformMat4(const std::string &name, glm::mat4 value) {
    int loc = glGetUniformLocation(program_id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
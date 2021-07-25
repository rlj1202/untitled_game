#define LUVOASI_DEBUG_ENABLE

#include "platforms/luvoasi/graphics/common_opengl.h"
#include "platforms/luvoasi/graphics/shader_opengl.h"

namespace Luvoasi {

template<> const uint32_t OpenGLShaderType<ShaderType::UNKNOWN>::value = -1;
template<> const uint32_t OpenGLShaderType<ShaderType::VERTEX_SHADER>::value = GL_VERTEX_SHADER;
template<> const uint32_t OpenGLShaderType<ShaderType::FRAGMENT_SHADER>::value = GL_FRAGMENT_SHADER;

template<> const uint32_t OpenGLDataType<DataType::INT8>::value = GL_BYTE;
template<> const uint32_t OpenGLDataType<DataType::UINT8>::value = GL_UNSIGNED_BYTE;
template<> const uint32_t OpenGLDataType<DataType::INT16>::value = GL_SHORT;
template<> const uint32_t OpenGLDataType<DataType::UINT16>::value = GL_UNSIGNED_SHORT;
template<> const uint32_t OpenGLDataType<DataType::INT32>::value = GL_INT;
template<> const uint32_t OpenGLDataType<DataType::UINT32>::value = GL_UNSIGNED_INT;
template<> const uint32_t OpenGLDataType<DataType::INT64>::value = 0;
template<> const uint32_t OpenGLDataType<DataType::UINT64>::value = 0;

template<> const uint32_t OpenGLDataType<DataType::FLOAT16>::value = GL_HALF_FLOAT;
template<> const uint32_t OpenGLDataType<DataType::FLOAT32>::value = GL_FLOAT;
template<> const uint32_t OpenGLDataType<DataType::FLOAT64>::value = 0;
template<> const uint32_t OpenGLDataType<DataType::FIXED32>::value = GL_FIXED;
template<> const uint32_t OpenGLDataType<DataType::INT_2_10_10_10_REV>::value = GL_INT_2_10_10_10_REV;
template<> const uint32_t OpenGLDataType<DataType::UINT_2_10_10_10_REV>::value = GL_UNSIGNED_INT_2_10_10_10_REV;

template<> const uint32_t OpenGLShaderPrimitive<ShaderPrimitive::BOOL>::value = GL_INT;
template<> const uint32_t OpenGLShaderPrimitive<ShaderPrimitive::INT>::value = GL_INT;
template<> const uint32_t OpenGLShaderPrimitive<ShaderPrimitive::FLOAT>::value = GL_FLOAT;

uint32_t GetOpenGLDataType(DataType::Value value) {
    switch (value) {
    case DataType::INT8: return OpenGLDataType<DataType::INT8>::value;
    case DataType::UINT8: return OpenGLDataType<DataType::UINT8>::value;
    case DataType::INT16: return OpenGLDataType<DataType::INT16>::value;
    case DataType::UINT16: return OpenGLDataType<DataType::UINT16>::value;
    case DataType::INT32: return OpenGLDataType<DataType::INT32>::value;
    case DataType::UINT32: return OpenGLDataType<DataType::UINT32>::value;
    case DataType::INT64: return OpenGLDataType<DataType::INT64>::value;
    case DataType::UINT64: return OpenGLDataType<DataType::UINT64>::value;
    
    case DataType::FLOAT16: return OpenGLDataType<DataType::FLOAT16>::value;
    case DataType::FLOAT32: return OpenGLDataType<DataType::FLOAT32>::value;
    case DataType::FLOAT64: return OpenGLDataType<DataType::FLOAT64>::value;

    default: return 0;
    }
}

OpenGLShader::OpenGLShader(uint32_t type, const char* source) {
    m_id = glCreateShader(type);
    glShaderSource(m_id, 1, &source, nullptr);
    glCompileShader(m_id);

    int success;
    char info_log[512];
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_id, sizeof(info_log), nullptr, info_log);
        LUVOASI_DEBUG_STDOUT("%s\n", info_log);
    }
}

OpenGLShader::OpenGLShader(OpenGLShader&& o) {
    m_id = o.m_id;
    o.m_id = 0;
}

OpenGLShader::~OpenGLShader() {
    if (m_id) {
        glDeleteShader(m_id);
        m_id = 0;
    }
}

OpenGLShader& OpenGLShader::operator=(OpenGLShader&& o) noexcept {
    if (this == &o) return *this;

    m_id = o.m_id;
    o.m_id = 0;

    return *this;
}

uint32_t OpenGLShader::GetId() const {
    return m_id;
}

OpenGLShaderProgram::OpenGLShaderProgram(std::initializer_list<Shader*> shaders) {
    m_id = glCreateProgram();

    for (const Shader* shader : shaders) {
        glAttachShader(m_id, shader->GetId());
    }
    glLinkProgram(m_id);

    int success;
    char info_log[512];
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, sizeof(info_log), nullptr, info_log);
        LUVOASI_DEBUG_STDOUT("%s\n", info_log);
    }
}

OpenGLShaderProgram::OpenGLShaderProgram(OpenGLShaderProgram&& o) {
    m_id = o.m_id;
    o.m_id = 0;
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
    if (m_id) {
        glDeleteProgram(m_id);
        m_id = 0;
    }
}

OpenGLShaderProgram& OpenGLShaderProgram::operator=(OpenGLShaderProgram&& o) noexcept {
    if (this == &o) return *this;

    m_id = o.m_id;
    o.m_id = 0;

    return *this;
}

uint32_t OpenGLShaderProgram::GetId() const {
    return m_id;
}

int32_t OpenGLShaderProgram::GetUniformLocation(const char* name) const {
    return glGetUniformLocation(m_id, name);
}

void OpenGLShaderProgram::SetUniform1i(int32_t loc, int a) {
    glUniform1i(loc, a);
}

void OpenGLShaderProgram::SetUniform2i(int32_t loc, int a, int b) {
    glUniform2i(loc, a, b);
}

void OpenGLShaderProgram::SetUniform3i(int32_t loc, int a, int b, int c) {
    glUniform3i(loc, a, b, c);
}

void OpenGLShaderProgram::SetUniform4i(int32_t loc, int a, int b, int c, int d) {
    glUniform4i(loc, a, b, c, d);
}

void OpenGLShaderProgram::SetUniformMat4fv(int32_t loc, int count, bool transpose, const float* values) {
    glUniformMatrix4fv(loc, count, transpose, values);
}

void OpenGLShaderProgram::Bind() const {
    glUseProgram(m_id);
}

void OpenGLShaderProgram::Unbind() const {
    glUseProgram(0);
}

std::unique_ptr<Shader> Shader::CreateShader(ShaderType type, const char* source) {
    uint32_t gl_type = OpenGLShaderType<ShaderType::VERTEX_SHADER>::value;

    if (type == ShaderType::VERTEX_SHADER)
        gl_type = OpenGLShaderType<ShaderType::VERTEX_SHADER>::value;
    if (type == ShaderType::FRAGMENT_SHADER)
        gl_type = OpenGLShaderType<ShaderType::FRAGMENT_SHADER>::value;
    
    return std::move(std::make_unique<OpenGLShader>(gl_type, source));
}

std::unique_ptr<ShaderProgram> ShaderProgram::CreateShaderProgram(std::initializer_list<Shader*> shaders) {
    return std::move(std::make_unique<OpenGLShaderProgram>(shaders));
}

}

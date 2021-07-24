#ifndef H_LUVOASI_SHADER_OPENGL
#define H_LUVOASI_SHADER_OPENGL

#ifdef EMSCRIPTEN
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "luvoasi/core/types.h"
#include "luvoasi/graphics/shader.h"

namespace Luvoasi {

template<ShaderType T>
class OpenGLShaderType {
public:
    static const uint32_t value;
};

template<DataType::Value T>
class OpenGLDataType {
public:
    static const uint32_t value;
};
uint32_t GetOpenGLDataType(DataType::Value value);

template<ShaderPrimitive::Value T>
class OpenGLShaderPrimitive {
public:
    static const uint32_t value;
};

class OpenGLShader : public Shader {
public:
    OpenGLShader(uint32_t type, const char* source);
    OpenGLShader(const OpenGLShader& o) = delete;
    OpenGLShader(OpenGLShader&& o);
    virtual ~OpenGLShader();

    OpenGLShader& operator=(const OpenGLShader& o) = delete;
    OpenGLShader& operator=(OpenGLShader&& o) noexcept;

    uint32_t GetId() const override;

private:
    uint32_t m_id;
};

class OpenGLShaderProgram : public ShaderProgram {
public:
    OpenGLShaderProgram(std::initializer_list<Shader*> shaders);
    OpenGLShaderProgram(const OpenGLShaderProgram& o) = delete;
    OpenGLShaderProgram(OpenGLShaderProgram&& o);
    virtual ~OpenGLShaderProgram();

    OpenGLShaderProgram& operator=(const OpenGLShaderProgram& o) = delete;
    OpenGLShaderProgram& operator=(OpenGLShaderProgram&& o) noexcept;

    uint32_t GetId() const override;

    int32_t GetUniformLocation(const char* name) const override;

    void SetUniform1i(int32_t loc, int a) override;
    void SetUniform2i(int32_t loc, int a, int b) override;
    void SetUniform3i(int32_t loc, int a, int b, int c) override;
    void SetUniform4i(int32_t loc, int a, int b, int c, int d) override;
    void SetUniformMat4fv(int32_t loc, int count, bool transpose, const float* values) override;
    
    void Bind() const override;
    void Unbind() const override;

private:
    uint32_t m_id;
};

}

#endif
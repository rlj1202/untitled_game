#ifndef H_LUVOASI_SHADER
#define H_LUVOASI_SHADER

#include <memory>

#include "luvoasi/core/base.h"

namespace Luvoasi {

enum class ShaderType {
    UNKNOWN,

    VERTEX_SHADER,
    FRAGMENT_SHADER,
    GEOMETRY_SHADER,
    COMPUTE_SHADER,
};

class ShaderPrimitive {
public:
    enum Value {
        BOOL,
        INT,
        FLOAT,
    };
};

class Shader {
public:
    virtual ~Shader() = default;

    virtual uint32_t GetId() const = 0;

    /**
     * @brief Create shader from source code.
     * @details
     * 
     * @param type
     * @param source
     */
    static std::unique_ptr<Shader> CreateShader(ShaderType type, const char* source);

    static std::unique_ptr<Shader> CreateShaderFromFile(ShaderType type, const char* path);
};

class ShaderProgram {
public:
    virtual ~ShaderProgram() = default;

    virtual uint32_t GetId() const = 0;

    virtual int32_t GetUniformLocation(const char* name) const = 0;

    virtual void SetUniform1i(int32_t loc, int a) = 0;
    virtual void SetUniform2i(int32_t loc, int a, int b) = 0;
    virtual void SetUniform3i(int32_t loc, int a, int b, int c) = 0;
    virtual void SetUniform4i(int32_t loc, int a, int b, int c, int d) = 0;
    virtual void SetUniformMat4fv(int32_t loc, int count, bool transpose, const float* values) = 0;

    template<typename T>
    void SetUniform(int32_t loc, const T& value);

    template<typename T>
    void SetUniform(const char* name, const T& value);

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    /**
     * @brief Create shader program from shaders.
     * @details
     * 
     * @param shaders
     */
    static std::unique_ptr<ShaderProgram> CreateShaderProgram(std::initializer_list<Shader*> shaders);

    static std::unique_ptr<ShaderProgram> CreateShaderProgramFromFiles(const char* vertex_shader_path, const char* fragment_shader_path);
};

}
#include "luvoasi/graphics/shader.tcc"

#endif
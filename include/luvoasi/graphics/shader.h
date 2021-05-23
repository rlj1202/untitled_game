#ifndef H_LUVOASI_SHADER
#define H_LUVOASI_SHADER

namespace Luvoasi {

class Shader {
public:
    virtual ~Shader() = default;


};

class ShaderProgram {
public:
    virtual ~ShaderProgram() = default;

    virtual void AttachShader() = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
};

}

#endif
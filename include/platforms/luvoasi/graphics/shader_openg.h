#ifndef H_LUVOASI_SHADER_OPENGL
#define H_LUVOASI_SHADER_OPENGL

#include "luvoasi/graphics/shader.h"

namespace Luvoasi {

class OpenGLShader : public Shader {
public:
    virtual ~OpenGLShader();

};

class OpenGLShaderProgram : public ShaderProgram {
public:
    virtual ~OpenGLShaderProgram();

    void Bind() const override;
    void Unbind() const override;
};

}

#endif
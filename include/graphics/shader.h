#ifndef SHADER_H
#define SHADER_H

#include <cstdio>
#include <string>

#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

#include <glm/glm.hpp>

enum class Test {
    VEC2,
    VEC3,
    VEC4,
    MAT4,
};

template<Test T, typename U, typename V, unsigned int W>
class ShaderDataType {
public:
    const static unsigned int size;
};

template<>
class ShaderDataType<Test::MAT4, glm::mat4, float, GL_FLOAT> {
public:
    const static unsigned int size = 4 * 4;
};

class Shader {
public:
    Shader(GLenum type, const char* code);
    Shader(const Shader* o) = delete;
    Shader(Shader&& o);
    ~Shader();

    Shader& operator=(const Shader& o) = delete;
    Shader& operator=(Shader&& o) noexcept;

    unsigned int GetId() const;

private:
    unsigned int id;
};

class ShaderProgram {
public:
    ShaderProgram(std::initializer_list<Shader> shaders);
    ShaderProgram(const char* vertex_shader_code, const char* fragment_shader_code);
    ShaderProgram(const ShaderProgram& o) = delete;
    ShaderProgram(ShaderProgram&& o);
    ~ShaderProgram();

    ShaderProgram& operator=(const ShaderProgram& o) = delete;
    ShaderProgram& operator=(ShaderProgram&& o) noexcept;

    void Use();

    template<typename T>
    void SetUniform(const std::string& name, T value);

private:
    unsigned int program_id;
};

#endif
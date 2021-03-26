#ifndef SHADER_H
#define SHADER_H

#include <cstdio>
#include <string>

#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    Shader(const char *vertex_shader_code, const char *fragment_shader_code);
    Shader(const Shader &o) = delete;
    Shader(Shader &&o);
    ~Shader();

    void Use();

    void SetUniformBool(const std::string &name, bool value);
    void SetUniformInt(const std::string &name, int value);
    void SetUniformFloat(const std::string &name, float value);
    void SetUniformMat4(const std::string &name, glm::mat4 value);

private:
    unsigned int program_id;
};

#endif
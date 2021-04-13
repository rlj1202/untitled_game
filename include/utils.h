#ifndef H_UTILS
#define H_UTILS

#include <string>

#include "graphics/shader.h"
#include "graphics/texture.h"

Shader LoadShader(std::string vertex_shader, std::string fragment_shader);

Texture LoadTexture(std::string path);

#endif
#ifndef H_UTILS
#define H_UTILS

#include <string>

#include "graphics/shader.h"
#include "graphics/texture.h"

ShaderProgram LoadShader(std::string vertex_shader, std::string fragment_shader);

/**
 * @brief Get total momory size in bytes.
 */
unsigned long long GetMemoryTotal();

/**
 * @brief Get current memory usage in bytes.
 */
unsigned long long GetMemoryUsage();

#endif
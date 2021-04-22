#ifndef H_UTILS
#define H_UTILS

#include <string>

#include "graphics/shader.h"
#include "graphics/texture.h"

Shader LoadShader(std::string vertex_shader, std::string fragment_shader);

Texture LoadTexture(std::string path);

std::unique_ptr<TextureAtlas> LoadTextureAtlas(std::string path);

/**
 * @brief Get total momory size in bytes.
 */
unsigned long long GetMemoryTotal();

/**
 * @brief Get current memory usage in bytes.
 */
unsigned long long GetMemoryUsage();

#endif
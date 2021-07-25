#ifndef H_LUVOASI_GRAPHICS_COMMON_OPENGL
#define H_LUVOASI_GRAPHICS_COMMON_OPENGL

// opengl bindings
#if defined(EMSCRIPTEN)
#include <GLES3/gl3.h>
#elif defined(_WIN32)
#include <glad/glad.h>
#endif

#endif
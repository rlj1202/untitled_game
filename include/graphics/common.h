#ifndef H_GRAPHICS_COMMON
#define H_GRAPHICS_COMMON

#include <GLES3/gl3.h>

template<typename T>
class GLPrimitive {
public:
    const static unsigned int type;
};

template<> class GLPrimitive<char> { public: const static unsigned int type = GL_BYTE; };
template<> class GLPrimitive<unsigned char> { public: const static unsigned int type = GL_UNSIGNED_BYTE; };
template<> class GLPrimitive<short> { public: const static unsigned int type = GL_SHORT; };
template<> class GLPrimitive<unsigned short> { public: const static unsigned int type = GL_UNSIGNED_SHORT; };
template<> class GLPrimitive<int> { public: const static unsigned int type = GL_INT; };
template<> class GLPrimitive<unsigned int> { public: const static unsigned int type = GL_UNSIGNED_INT; };
template<> class GLPrimitive<float> { public: const static unsigned int type = GL_FLOAT; };

#endif
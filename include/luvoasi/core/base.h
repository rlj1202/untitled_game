#ifndef H_LUVOASI_BASE
#define H_LUVOASI_BASE

#include <cstdint> // for types such as uint32_t, int32_t etc...

namespace Luvoasi {

#ifdef LUVOASI_DEBUG_ENABLE
    #include <cstdio>
    #define LUVOASI_DEBUG_STDOUT(fmt, ...) \
        do { printf(fmt, ##__VA_ARGS__); } while (0)
    #define LUVOASI_DEBUG_STDERR(fmt, ...) \
        do { fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)
#else
    #define LUVOASI_DEBUG_STDOUT(fmt, ...) do {} while (0)
    #define LUVOASI_DEBUG_STDERR(fmt, ...) do {} while (0)
#endif

#ifdef LUVOASI_ASSERT_ENABLE
    #define LUVOASI_ASSERT(condition) \
        assert(condition);
#else
    #define LUVOASI_ASSERT(...) \
        do {} while (0)
#endif

}

#endif
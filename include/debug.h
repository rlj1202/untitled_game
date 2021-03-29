#ifndef H_DEBUG
#define H_DEBUG

#ifdef DEBUG
    #include <cstdio>
    #define DEBUG_STDOUT(fmt, ...) \
        do { printf(fmt, ##__VA_ARGS__); } while (0)
    #define DEBUG_STDERR(fmt, ...) \
        do { fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)
#else
    #define DEBUG_STDOUT(fmt, ...) do {} while (0)
    #define DEBUG_STDERR(fmt, ...) do {} while (0)
#endif

#endif
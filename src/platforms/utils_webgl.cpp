#include "utils.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

unsigned long long GetMemoryTotal() {
    return GetMemoryUsage();
}

unsigned long long GetMemoryUsage() {
    int total_bytes = EM_ASM_INT({
        return HEAP8.length;
    });

    return total_bytes;
}
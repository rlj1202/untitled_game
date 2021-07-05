#include <GLES3/gl3.h>

namespace Temp {

void interleaving() {
    int stride = 0;

    for (int index = 0; index < 4; index++) {
        int size = 4;

        stride += size;
    }

    int offset = 0;

    for (int index = 0; index < 4; index++) {
        int size = 4;
        bool normalized = false;

        glVertexAttribPointer(index, size, GL_UNSIGNED_BYTE, normalized, stride, (void*) offset);
        glEnableVertexAttribArray(index);

        offset += size;
    }
}

void massing() {
    int cnt_vertices = 0;

    int offset = 0;

    for (int index = 0; index < 4; index++) {
        int size = 4;
        bool normalzed = false;

        glVertexAttribPointer(index, size, GL_UNSIGNED_BYTE, normalzed, 0, (void*) offset);
        glEnableVertexAttribArray(index);

        offset += size * cnt_vertices;
    }
}

}

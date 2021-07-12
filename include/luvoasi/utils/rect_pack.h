#ifndef H_LUVOASI_RECT_PACK
#define H_LUVOASI_RECT_PACK

#include <glm/glm.hpp>

#define LUVOASI_DEBUG_ENABLE
#define LUVOASI_ASSERT_ENABLE
#include "luvoasi/core/base.h"

namespace Luvoasi {

/**
 * @brief Packs set of rectangles into one big rectangle as tight as possible.
 * @details
 * This class is implemented using Skyline-based Left Bottom Algorithm.
 * 
 * Informations about place where new rectangle can be placed are represented
 * as skyline.
 * 
 * If new rectangle comes in, the algorithm will find bottom-most and left-most
 * place and returns that result.
 */
class RectPack {
private:
    struct Node {
        unsigned int x, y;
        Node *prev, *next;

        Node();
        Node(unsigned int x, unsigned int y, Node* prev, Node* next);
        ~Node();
    };

public:
    RectPack(unsigned int width, unsigned int height);
    RectPack(const RectPack& o) = delete;
    RectPack(RectPack&& o);
    ~RectPack();

    RectPack& operator=(const RectPack& o) = delete;
    RectPack& operator=(RectPack&& o) noexcept;

    /**
     * @brief Acquire appropriate place for given area.
     * 
     * @return Coordinates of corner of acquired area, {-1, -1} if failed.
     */
    glm::ivec2 Claim(unsigned int width, unsigned int height);

private:
    glm::ivec2 FindBottomLeft(unsigned int width, unsigned int height, Node** best_node);
    unsigned int FindMinY(Node* node, unsigned int width);

    unsigned int m_width;
    unsigned int m_height;

    Node* m_head;
};

}

#endif
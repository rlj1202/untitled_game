#ifndef H_RECT_PACK
#define H_RECT_PACK

#include <glm/glm.hpp>

/**
 * Implemented using Skyline-based Left Bottom Algorithm.
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
    RectPack& operator=(RectPack&& o);

    /**
     * @brief Acquire appropriate place for given area.
     * 
     * @return Coordinates of corner of acquired area, {-1, -1} if failed.
     */
    glm::ivec2 Claim(unsigned int width, unsigned int height);

private:
    glm::ivec2 FindBottomLeft(unsigned int width, unsigned int height, Node** best_node);
    unsigned int FindMinY(Node* node, unsigned int width);

    unsigned int width;
    unsigned int height;

    Node* head;
};

#endif
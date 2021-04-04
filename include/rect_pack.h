#ifndef H_RECT_PACK
#define H_RECT_PACK

#include <glm/glm.hpp>

class RectPack {
private:
    struct Node {
        unsigned int x, y;
        Node *prev, *next;

        Node();
        Node(unsigned int x, unsigned int y, Node *prev, Node *next);
        ~Node();
    };

public:
    RectPack(unsigned int width, unsigned int height);
    RectPack(const RectPack &o) = delete;
    RectPack(RectPack &&o);
    ~RectPack();

    glm::ivec2 Claim(unsigned int width, unsigned int height);

private:
    glm::ivec2 FindBottomLeft(unsigned int width, unsigned int height, Node **best_node);
    unsigned int FindMinY(Node *node, unsigned int width);

    unsigned int width;
    unsigned int height;

    Node *head;
};

#endif
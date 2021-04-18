#include "rect_pack.h"

#include <cassert>

#define DEBUG
#include "debug.h"

RectPack::Node::Node() : RectPack::Node::Node(0, 0, nullptr, nullptr) {
}

RectPack::Node::Node(unsigned int x, unsigned int y, RectPack::Node *prev, RectPack::Node *next)
    : x(x), y(y), prev(prev), next(next) {
}

RectPack::Node::~Node() {
    if (next) {
        delete next;
        next = nullptr;
    }
    prev = nullptr;
}

RectPack::RectPack(unsigned int width, unsigned int height) : width(width), height(height) {
    head = new RectPack::Node(0, 0, nullptr, nullptr);
    head->next = new RectPack::Node(0, 0, head, nullptr);

    assert(head);
    assert(head->next);
    assert(head->next->prev);
}

RectPack::RectPack(RectPack &&o) {
    width = o.width;
    height = o.height;

    head = o.head;
    o.head = nullptr;
}

RectPack::~RectPack() {
    if (head) {
        delete head;
        head = nullptr;
    }
}

RectPack& RectPack::operator=(RectPack&& o) {
    width = o.width;
    height = o.height;

    head = o.head;
    o.head = nullptr;

    return *this;
}

glm::ivec2 RectPack::Claim(unsigned int width, unsigned int height) {
    assert(width <= this->width);
    assert(height <= this->height);
    
    Node *best_node = nullptr;
    glm::ivec2 result = FindBottomLeft(width, height, &best_node);

    if (best_node && result.y + height <= this->height) {
        Node *new_node = new Node(result.x, result.y + height, nullptr, nullptr);
        assert(new_node);

        Node *prev = best_node->prev;
        assert(prev);
        prev->next = new_node;
        new_node->prev = prev;
        
        Node *cur = best_node;
        while (cur) {
            int right = cur->next ? cur->next->x : this->width;
            if (right >= new_node->x + width) {
                break;
            }
            // DEBUG_STDOUT("delete node : (%d %d), right = %d\n", cur->x, cur->y, right);
            Node *next = cur->next;
            cur->next = nullptr;
            cur->prev = nullptr;
            delete cur;
            cur = next;
        }

        new_node->next = cur;
        if (cur) cur->prev = new_node;

        if (cur) cur->x = new_node->x + width;

        return result;
    }

    // TextureAtlasNode *test = head->next;
    // int chain_length = 0;
    // while (test) {
    //     chain_length++;
    //     test = test->next;
    // }
    // DEBUG_STDOUT("chain length : %d\n", chain_length);

    return {-1, -1};
}

glm::ivec2 RectPack::FindBottomLeft(unsigned int width, unsigned int height, Node **best_node) {
    assert(width <= this->width);
    assert(height <= this->height);

    Node* cur = head->next;
    assert(cur);
    assert(cur->prev);

    int best_y = this->height; // init to maximum value

    *best_node = nullptr;

    while (cur) {
        if (this->width - cur->x < width) break;
        
        int min_y = FindMinY(cur, width);

        if (best_y > min_y) {
            best_y = min_y;
            *best_node = cur;
        }

        cur = cur->next;
    }

    // DEBUG_STDOUT("FindBottomLeft: (%d %d)\n", width, height);
    // if (*best_node) DEBUG_STDOUT("\t(%d %d)\n", (*best_node)->x, (*best_node)->y);

    if (*best_node) return {(*best_node)->x, best_y};
    else return {-1, -1};
}

unsigned int RectPack::FindMinY(Node *node, unsigned int width) {
    assert(node);
    assert(width <= this->width);

    int min_y = 0;
    int left = node->x;
    int right = left + width;

    Node* cur = node;

    while (cur) {
        if (cur->x < right) {
            if (min_y < cur->y) {
                min_y = cur->y;
            }
        } else {
            break;
        }

        cur = cur->next;
    }

    // DEBUG_STDOUT("FindMinY: %d for width %d\n", min_y, width);
    
    return min_y;
}
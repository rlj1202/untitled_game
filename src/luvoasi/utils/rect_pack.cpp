#include "luvoasi/utils/rect_pack.h"

namespace Luvoasi {

RectPack::Node::Node() : RectPack::Node::Node(0, 0, nullptr, nullptr) {
}

RectPack::Node::Node(
    unsigned int x, unsigned int y,
    RectPack::Node *prev, RectPack::Node *next)
    : x(x), y(y), prev(prev), next(next) {
}

RectPack::Node::~Node() {
    if (next) {
        delete next;
        next = nullptr;
    }
    prev = nullptr;
}

RectPack::RectPack(unsigned int width, unsigned int height)
    : m_width(width), m_height(height) {
    m_head = new RectPack::Node(0, 0, nullptr, nullptr);
    m_head->next = new RectPack::Node(0, 0, m_head, nullptr);

    LUVOASI_ASSERT(m_head);
    LUVOASI_ASSERT(m_head->next);
    LUVOASI_ASSERT(m_head->next->prev);
}

RectPack::RectPack(RectPack &&o) {
    m_width = o.m_width;
    m_height = o.m_height;

    m_head = o.m_head;
    o.m_head = nullptr;
}

RectPack::~RectPack() {
    if (m_head) {
        delete m_head;
        m_head = nullptr;
    }
}

RectPack& RectPack::operator=(RectPack&& o) noexcept {
    m_width = o.m_width;
    m_height = o.m_height;

    m_head = o.m_head;
    o.m_head = nullptr;

    return *this;
}

glm::ivec2 RectPack::Claim(unsigned int width, unsigned int height) {
    LUVOASI_ASSERT(width <= this->m_width);
    LUVOASI_ASSERT(height <= this->m_height);
    
    Node *best_node = nullptr;
    glm::ivec2 result = FindBottomLeft(width, height, &best_node);

    if (best_node && result.y + height <= this->m_height) {
        Node *new_node = new Node(
            result.x, result.y + height,
            nullptr, nullptr);
        LUVOASI_ASSERT(new_node);

        Node *prev = best_node->prev;
        LUVOASI_ASSERT(prev);
        prev->next = new_node;
        new_node->prev = prev;
        
        Node *cur = best_node;
        while (cur) {
            int right = cur->next ? cur->next->x : this->m_width;
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

glm::ivec2 RectPack::FindBottomLeft(
    unsigned int width, unsigned int height, Node **best_node) {
    LUVOASI_ASSERT(width <= this->m_width);
    LUVOASI_ASSERT(height <= this->m_height);

    Node* cur = m_head->next;
    LUVOASI_ASSERT(cur);
    LUVOASI_ASSERT(cur->prev);

    int best_y = this->m_height; // init to maximum value

    *best_node = nullptr;

    while (cur) {
        if (this->m_width - cur->x < width) break;
        
        int min_y = FindMinY(cur, width);

        if (best_y > min_y) {
            best_y = min_y;
            *best_node = cur;
        }

        cur = cur->next;
    }

    // DEBUG_STDOUT("FindBottomLeft: (%d %d)\n", width, height);
    // if (*best_node)
    //     DEBUG_STDOUT("\t(%d %d)\n", (*best_node)->x, (*best_node)->y);

    if (*best_node) return {(*best_node)->x, best_y};
    else return {-1, -1};
}

unsigned int RectPack::FindMinY(Node *node, unsigned int width) {
    LUVOASI_ASSERT(node);
    LUVOASI_ASSERT(width <= this->m_width);

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

}
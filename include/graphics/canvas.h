#ifndef H_CANVAS
#define H_CANVAS

#include <vector>
#include <memory>

#include "graphics/mesh.h"

/**
 * @class Canvas
 * @brief Provides batch rendering
 * 
 * `Canvas` class provides function for batch rendering.
 */
class Canvas {
public:
    Canvas();

    void Draw(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void DrawLine();
    void DrawRect();

    void Render();
    void Clear();

private:
    std::unique_ptr<Mesh> mesh;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

#endif
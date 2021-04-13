#include "graphics/gui.h"

#include "graphics/mesh_builder.h"

bool GuiArea::IsIn(const glm::vec2 pos) {
    return x <= pos.x && pos.x < x + width &&
           y <= pos.y && pos.y < y + height;
}

void IGuiLayout::AppendChild(IGuiNode* node) {
    children.push_back(node);
}

GuiSimpleLayout::GuiSimpleLayout(GuiArea area, Texture* texture, int corner)
    : area(area), texture(texture), corner(corner) {
}

GuiArea GuiSimpleLayout::Draw(Canvas& canvas, GuiArea area) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int x_body = this->area.width - corner * 2;
    int y_body = this->area.height - corner * 2;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            /* 
             * 0     1                       2     3
             * |-----|-----------------------|-----|
             *  corner          body
             */
            int x = 0;
            int y = 0;

            if (c == 0) x = 0;
            if (c == 1) x = corner;
            if (c == 2) x = corner + x_body;
            if (c == 3) x = corner * 2 + x_body;

            if (r == 0) y = 0;
            if (r == 1) y = corner;
            if (r == 2) y = corner + y_body;
            if (r == 3) y = corner * 2 + y_body;

            vertices.push_back(Vertex(
                glm::vec3(x + this->area.x, y + this->area.y, 0.0f),
                glm::vec2(c / 3.0f, (3 - r) / 3.0f)
            ));
        }
    }
    for (int r = 1; r < 4; r++) {
        for (int c = 1; c < 4; c++) {
            int index = c + r * 4;

            indices.push_back(index);
            indices.push_back(index - 4 - 1);
            indices.push_back(index - 4);

            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(index - 4 - 1);
        }
    }

    canvas.Draw(vertices, indices);

    return {};
}

GuiArea& GuiSimpleLayout::GetArea() {
    return area;
}
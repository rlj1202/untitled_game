#include "graphics/gui.h"

#include <sstream>

#include "graphics/mesh_builder.h"

bool GuiArea::IsIn(const glm::vec2 pos) {
    return x <= pos.x && pos.x < x + width &&
           y <= pos.y && pos.y < y + height;
}

IGuiNode::IGuiNode(GuiArea area) : IGuiNode(nullptr, area) {
}

IGuiNode::IGuiNode(IGuiNode&& o) {
    parent = o.parent;
    area = o.area;

    children = std::move(o.children);
}

IGuiNode::IGuiNode(IGuiNode* parent, GuiArea area) : parent(parent), area(area) {
}

void IGuiNode::AppendChild(std::unique_ptr<IGuiNode> node) {
    node->parent = this;
    children.push_back(std::move(node));
}

std::vector<std::unique_ptr<IGuiNode>>& IGuiNode::GetChildren() {
    return children;
}

IGuiNode* IGuiNode::GetParent() {
    return parent;
}

GuiArea& IGuiNode::GetArea() {
    return area;
}

GuiArea IGuiNode::GetAbsoluteArea() {
    GuiArea result = area;
    if (parent) {
        GuiArea parent_area = parent->GetAbsoluteArea();
        result.x += parent_area.x;
        result.y += parent_area.y;
    }
    return result;
}

GuiRect::GuiRect(GuiArea area, Texture* texture, int corner)
    : IGuiNode(nullptr, area), texture(texture), corner(corner) {
}

void GuiRect::Draw(Canvas& canvas) {
    GuiArea area = GetAbsoluteArea();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(16);
    indices.reserve(16);

    int x_body = area.width - corner * 2;
    int y_body = area.height - corner * 2;

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

            vertices.emplace_back(
                glm::vec3(x + area.x, y + area.y, 0.0f),
                glm::vec2(c / 3.0f, r / 3.0f)
            );
            // vertices.push_back(Vertex(
            //     glm::vec3(x + area.x, y + area.y, 0.0f),
            //     glm::vec2(c / 3.0f, r / 3.0f)
            // ));
        }
    }
    for (int r = 1; r < 4; r++) {
        for (int c = 1; c < 4; c++) {
            int index = c + r * 4;

            indices.emplace_back(index);
            indices.emplace_back(index - 4 - 1);
            indices.emplace_back(index - 4);

            indices.emplace_back(index);
            indices.emplace_back(index - 1);
            indices.emplace_back(index - 4 - 1);

            // indices.push_back(index);
            // indices.push_back(index - 4 - 1);
            // indices.push_back(index - 4);

            // indices.push_back(index);
            // indices.push_back(index - 1);
            // indices.push_back(index - 4 - 1);
        }
    }

    canvas.Draw(texture, vertices, indices);
}

GuiWindow::GuiWindow(GuiArea area, Texture* texture, int corner)
    : GuiRect(area, texture, corner) {

}

bool GuiWindow::OnMouseDrag(glm::vec2 pos, glm::vec2 rel, int button, int modifiers) {
    if (button == GLFW_MOUSE_BUTTON_1) {
        area.x += rel.x;
        area.y += rel.y;
    }

    return true;
}

GuiText::GuiText(GuiArea area, std::wstring text)
    : IGuiNode(area), text(text) {

}

void GuiText::Draw(Canvas& canvas) {
    GuiArea area = GetAbsoluteArea();

    int line = 0;
    int i = 0;
    while (i < text.size()) {
        int height = 16 + (16 + line_gap) * line;
        if (height >= area.height) break;

        int written = canvas.DrawText(
            glm::vec3(
                area.x,
                area.y + height,
                0.0f
            ),
            text.substr(i),
            area.width
        );

        if (!written) break;

        if (!auto_newline) break;
        line++;
        i += written;
    }
}

GuiButton::GuiButton(GuiArea area, Texture* texture, int corner)
    : GuiRect(area, texture, corner) {
}

void GuiButton::Draw(Canvas& canvas) {
    GuiRect::Draw(canvas);

    std::wostringstream stream;
    stream << L"count : " << test_count;

    GuiArea area = GetAbsoluteArea();
    canvas.Flush();
    canvas.DrawText(glm::vec3(area.x + 10, area.y + 16 + 5, 0), stream.str(), area.width);
}

bool GuiButton::OnMouseButton(glm::vec2 pos, int button, int action, int modifiers) {
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
        test_count++;

    return true;
}
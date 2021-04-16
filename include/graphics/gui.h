#ifndef H_GUI
#define H_GUI

#include <vector>

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/canvas.h"

class GuiArea {
public:
    int x, y;
    int width, height;

    bool IsIn(const glm::vec2 pos);
};

class IGuiNode {
public:
    IGuiNode(GuiArea area);
    virtual ~IGuiNode() = default;

    void AppendChild(IGuiNode* node);
    std::vector<IGuiNode*> GetChildren();

    GuiArea& GetArea();

    virtual GuiArea Draw(Canvas& canvas, GuiArea area) = 0;

    virtual bool OnKeyboard(int code, int key, int action, int modifiers) { return false; }
    virtual bool OnMouseDrag(glm::vec2 pos, glm::vec2 rel, int button, int modifiers) { return false; }
    virtual bool OnMouseButton(glm::vec2 pos, int button, int action, int modifiers) { return false; }
    virtual bool OnScroll(float x, float y) { return false; }

protected:
    GuiArea area;

    std::vector<IGuiNode*> children;
};

class GuiSimpleLayout : public IGuiNode {
public:
    GuiSimpleLayout(GuiArea area, Texture* texture, int corner);
    GuiSimpleLayout(const GuiSimpleLayout &o) = delete;
    GuiSimpleLayout(GuiSimpleLayout &&o) = default;
    ~GuiSimpleLayout() = default;

    GuiSimpleLayout& operator=(const GuiSimpleLayout &o) = delete;
    GuiSimpleLayout& operator=(GuiSimpleLayout &&o) = default;

    GuiArea Draw(Canvas& canvas, GuiArea area) override;

    bool OnMouseDrag(glm::vec2 pos, glm::vec2 rel, int button, int modifiers) override;

private:
    Texture* texture;

    int corner;
};

#endif
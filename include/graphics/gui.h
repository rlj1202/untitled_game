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
    IGuiNode(const IGuiNode& o) = delete;
    IGuiNode(IGuiNode&& o);
    IGuiNode(IGuiNode* parent, GuiArea area);
    virtual ~IGuiNode() = default;

    void AppendChild(std::unique_ptr<IGuiNode> node);
    std::vector<std::unique_ptr<IGuiNode>>& GetChildren();

    IGuiNode* GetParent();

    GuiArea& GetArea();
    GuiArea GetAbsoluteArea();

    virtual void Draw(Canvas& canvas) = 0;

    virtual bool OnKeyboard(int code, int key, int action, int modifiers) { return false; }
    virtual bool OnMouseDrag(glm::vec2 pos, glm::vec2 rel, int button, int modifiers) { return false; }
    virtual bool OnMouseButton(glm::vec2 pos, int button, int action, int modifiers) { return false; }
    virtual bool OnScroll(float x, float y) { return false; }

protected:
    IGuiNode* parent;
    GuiArea area;

    std::vector<std::unique_ptr<IGuiNode>> children;
};

class GuiRect : public IGuiNode {
public:
    GuiRect(GuiArea area, Texture* Texture, int corner);

    void Draw(Canvas& canvas) override;

protected:
    int corner;

private:
    Texture* texture;
};

class GuiWindow : public GuiRect {
public:
    GuiWindow(GuiArea area, Texture* texture, int corner);

    bool OnMouseDrag(glm::vec2 pos, glm::vec2 rel, int button, int modifiers) override;
};

class GuiText : public IGuiNode {
public:
    GuiText(GuiArea area, std::wstring text);

    void Draw(Canvas& canvas) override;

private:
    std::wstring text;

    bool auto_newline = true;
    int line_gap = 4;
};

class GuiButton : public GuiRect {
public:
    GuiButton(GuiArea area, Texture* texture, int corner);

    void Draw(Canvas& canvas) override;

    bool OnMouseButton(glm::vec2 pos, int button, int action, int modifiers) override;

private:
    int test_count = 0;
};

#endif
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
    virtual GuiArea Draw(Canvas& canvas, GuiArea area) = 0;
};

class IGuiLayout : public IGuiNode {
public:
    void AppendChild(IGuiNode* node);

protected:
    std::vector<IGuiNode*> children;
};

class GuiSimpleLayout : public IGuiLayout {
public:
    GuiSimpleLayout(GuiArea area, Texture* texture, int corner);
    GuiSimpleLayout(const GuiSimpleLayout &o) = delete;
    GuiSimpleLayout(GuiSimpleLayout &&o) = default;
    ~GuiSimpleLayout() = default;

    GuiSimpleLayout& operator=(const GuiSimpleLayout &o) = delete;
    GuiSimpleLayout& operator=(GuiSimpleLayout &&o) = default;

    GuiArea Draw(Canvas& canvas, GuiArea area) override;
    GuiArea& GetArea();

private:
    GuiArea area;

    Texture* texture;

    int corner;
};

#endif
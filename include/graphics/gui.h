#ifndef H_GUI
#define H_GUI

#include <vector>

class GuiArea {
public:
    int x, y, width, height;
};

class IGuiNode {
public:
    void AppendChild(IGuiNode* child);

    virtual GuiArea Draw(GuiArea area) = 0;

protected:
    std::vector<IGuiNode*> children;
};

class GuiSimpleLayout : public IGuiNode {
    void AppendChild(int x, int y, IGuiNode* child) {

    }
};

class GuiVerticalLayout : public IGuiNode {
    virtual GuiArea Draw(GuiArea area) override {


        for (auto child : children) {
            child->Draw(area);
        }

        return area;
    }
};

#endif
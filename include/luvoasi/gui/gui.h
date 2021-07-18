#ifndef H_LUVOASI_GUI
#define H_LUVOASI_GUI

#include <string>
#include <memory>
#include <vector>

#include "luvoasi/core/base.h"

namespace Luvoasi::GUI {

/**
 * @brief 
 * 
 */
class Component {
public:
    virtual ~Component() = default;

    virtual void Draw() const = 0;

    virtual void OnMouse();
    virtual void OnScroll(float dx, float dy);
    virtual void OnKeyboard();
    virtual void OnCursor(uint32_t x, uint32_t y);
    virtual void OnCharacter();

private:
    // position
    uint32_t m_x, m_y;

    // size
    uint32_t m_width, m_height;
};

class Container : public Component {
public:
    Component* AddChild(std::unique_ptr<Component> component);

private:
    std::vector<std::unique_ptr<Component>> m_children;
};

class BoxComponent : public Component {
public:
    void Draw() const override;

private:
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_corner;
    uint32_t m_padding;
};

class ButtonComponent : public Component {
public:
    void Draw() const override;

private:
    
};

class LabelComponent : public Component {
public:
    LabelComponent(const std::string& text);

    void Draw() const override;

private:
    std::string m_text;
};

}

#endif

#ifndef H_LUVOASI_WINDOW
#define H_LUVOASI_WINDOW

#include "luvoasi/core/base.h"
#include "luvoasi/event.h"

#include <memory>

namespace Luvoasi {

class Window;
class KeyEvent : public Event {
public:
    KeyEvent(Window* window, int key, int scancode, int action, int modifiers);

    std::wstring GetName() const override;

    Window* GetWindow() const;
    int GetKey() const;
    int GetScancode() const;
    int GetAction() const;
    int GetModifiers() const;

private:
    Window* m_window;
    int m_key;
    int m_scancode;
    int m_action;
    int m_modifiers;
};

class ScrollEvent : public Event {
public:
    ScrollEvent(Window* window, double x_offset, double y_offset);

    std::wstring GetName() const override;

    Window* GetWindow() const;
    double GetXOffset() const;
    double GetYOffset() const;

private:
    Window* m_window;
    double m_x_offset;
    double m_y_offset;
};

class MouseButtonEvent : public Event {
public:
    MouseButtonEvent(Window* window, int button, int action, int modifiers);

    std::wstring GetName() const override;

    Window* GetWindow() const;
    int GetButton() const;
    int GetAction() const;
    int GetModifiers() const;

private:
    Window* m_window;
    int m_button;
    int m_action;
    int m_mods;
};

class CursorPosEvent : public Event {
public:
    CursorPosEvent(Window* window, double x, double y);

    std::wstring GetName() const override;

    Window* GetWindow() const;
    double GetX() const;
    double GetY() const;

private:
    Window* m_window;
    double m_x;
    double m_y;
};

using KeyEventHandler = EventHandler<KeyEvent&>;
using ScrollEventHandler = EventHandler<ScrollEvent&>;
using MouseButtonEventHandler = EventHandler<MouseButtonEvent&>;
using CursorPosEventHandler = EventHandler<CursorPosEvent&>;

class Window {
public:
    virtual ~Window() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void GetCursorPos(double* x, double* y) const = 0;
    virtual uint32_t GetKey(uint32_t key) const = 0;
    virtual uint32_t GetMouseButton(uint32_t button) const = 0;

    virtual bool ShouldClose() const = 0;

    virtual void SwapBuffers() const = 0;
    virtual void PollEvents() const = 0;

    virtual void AddKeyEventHandler(KeyEventHandler handler) = 0;
    virtual void AddScrollEventHandler(ScrollEventHandler handler) = 0;
    virtual void AddMouseButtonEventHandler(MouseButtonEventHandler handler) = 0;
    virtual void AddCursorPosEventHandler(CursorPosEventHandler handler) = 0;

    virtual void Bind() const = 0;
};

std::unique_ptr<Window> CreateWindow(uint32_t width, uint32_t height, const char* title);

}

#endif
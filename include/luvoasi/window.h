#ifndef H_LUVOASI_WINDOW
#define H_LUVOASI_WINDOW

#include <GLFW/glfw3.h>

#include "luvoasi/core/base.h"
#include "luvoasi/event.h"

#include <memory>

namespace Luvoasi {

#define ENUM_KEY(key) key = GLFW_KEY_##key
#define ENUM_KEY_2(key) KEY_##key = GLFW_KEY_##key
#define ENUM_MOUSE_BUTTON(button) BUTTON##button = GLFW_MOUSE_BUTTON_##button

enum class Key : int32_t {
    ENUM_KEY(UNKNOWN),

    ENUM_KEY(SPACE),
    ENUM_KEY(APOSTROPHE),
    ENUM_KEY(COMMA),
    ENUM_KEY(MINUS),
    ENUM_KEY(PERIOD),
    ENUM_KEY(SLASH),
    ENUM_KEY_2(0),
    ENUM_KEY_2(1),
    ENUM_KEY_2(2),
    ENUM_KEY_2(3),
    ENUM_KEY_2(4),
    ENUM_KEY_2(5),
    ENUM_KEY_2(6),
    ENUM_KEY_2(7),
    ENUM_KEY_2(8),
    ENUM_KEY_2(9),
    ENUM_KEY(SEMICOLON),
    ENUM_KEY(EQUAL),

    ENUM_KEY(A),
    ENUM_KEY(B),
    ENUM_KEY(C),
    ENUM_KEY(D),
    ENUM_KEY(E),
    ENUM_KEY(F),
    ENUM_KEY(G),
    ENUM_KEY(H),
    ENUM_KEY(I),
    ENUM_KEY(J),
    ENUM_KEY(K),
    ENUM_KEY(L),
    ENUM_KEY(M),
    ENUM_KEY(N),
    ENUM_KEY(O),
    ENUM_KEY(P),
    ENUM_KEY(Q),
    ENUM_KEY(R),
    ENUM_KEY(S),
    ENUM_KEY(T),
    ENUM_KEY(U),
    ENUM_KEY(V),
    ENUM_KEY(W),
    ENUM_KEY(X),
    ENUM_KEY(Y),
    ENUM_KEY(Z),
    ENUM_KEY(LEFT_BRACKET),
    ENUM_KEY(BACKSLASH),
    ENUM_KEY(RIGHT_BRACKET),
    ENUM_KEY(GRAVE_ACCENT),
    ENUM_KEY(WORLD_1),
    ENUM_KEY(WORLD_2),

    ENUM_KEY(ESCAPE),
    ENUM_KEY(ENTER),
    ENUM_KEY(TAB),
    ENUM_KEY(BACKSPACE),
    ENUM_KEY(INSERT),
    ENUM_KEY(DELETE),
    ENUM_KEY(RIGHT),
    ENUM_KEY(LEFT),
    ENUM_KEY(DOWN),
    ENUM_KEY(UP),
    ENUM_KEY(PAGE_UP),
    ENUM_KEY(PAGE_DOWN),
    ENUM_KEY(HOME),
    ENUM_KEY(END),
    ENUM_KEY(CAPS_LOCK),
    ENUM_KEY(SCROLL_LOCK),
    ENUM_KEY(NUM_LOCK),
    ENUM_KEY(PRINT_SCREEN),
    ENUM_KEY(PAUSE),
    ENUM_KEY(F1),
    ENUM_KEY(F2),
    ENUM_KEY(F3),
    ENUM_KEY(F4),
    ENUM_KEY(F5),
    ENUM_KEY(F6),
    ENUM_KEY(F7),
    ENUM_KEY(F8),
    ENUM_KEY(F9),
    ENUM_KEY(F10),
    ENUM_KEY(F11),
    ENUM_KEY(F12),
    ENUM_KEY(F13),
    ENUM_KEY(F14),
    ENUM_KEY(F15),
    ENUM_KEY(F16),
    ENUM_KEY(F17),
    ENUM_KEY(F18),
    ENUM_KEY(F19),
    ENUM_KEY(F20),
    ENUM_KEY(F21),
    ENUM_KEY(F22),
    ENUM_KEY(F23),
    ENUM_KEY(F24),
    ENUM_KEY(F25),
    ENUM_KEY(KP_0),
    ENUM_KEY(KP_1),
    ENUM_KEY(KP_2),
    ENUM_KEY(KP_3),
    ENUM_KEY(KP_4),
    ENUM_KEY(KP_5),
    ENUM_KEY(KP_6),
    ENUM_KEY(KP_7),
    ENUM_KEY(KP_8),
    ENUM_KEY(KP_9),
    ENUM_KEY(KP_DECIMAL),
    ENUM_KEY(KP_DIVIDE),
    ENUM_KEY(KP_MULTIPLY),
    ENUM_KEY(KP_SUBTRACT),
    ENUM_KEY(KP_ADD),
    ENUM_KEY(KP_ENTER),
    ENUM_KEY(KP_EQUAL),
    ENUM_KEY(LEFT_SHIFT),
    ENUM_KEY(LEFT_CONTROL),
    ENUM_KEY(LEFT_ALT),
    ENUM_KEY(LEFT_SUPER),
    ENUM_KEY(RIGHT_SHIFT),
    ENUM_KEY(RIGHT_CONTROL),
    ENUM_KEY(RIGHT_ALT),
    ENUM_KEY(RIGHT_SUPER),
    ENUM_KEY(MENU),

    ENUM_KEY(LAST),
};

enum class MouseButton : int32_t {
    ENUM_MOUSE_BUTTON(1),
    ENUM_MOUSE_BUTTON(2),
    ENUM_MOUSE_BUTTON(3),
    ENUM_MOUSE_BUTTON(4),
    ENUM_MOUSE_BUTTON(5),
    ENUM_MOUSE_BUTTON(6),
    ENUM_MOUSE_BUTTON(7),
    ENUM_MOUSE_BUTTON(8),
    ENUM_MOUSE_BUTTON(LAST),
    ENUM_MOUSE_BUTTON(LEFT),
    ENUM_MOUSE_BUTTON(RIGHT),
    ENUM_MOUSE_BUTTON(MIDDLE),
};

enum class KeyState : int32_t {
    RELEASE = GLFW_RELEASE,
    PRESS = GLFW_PRESS,
    REPEAT = GLFW_REPEAT,
};

class Window;
class KeyEvent : public Event {
public:
    KeyEvent(Window* window, Key key, int scancode, KeyState action, int modifiers);

    std::wstring GetName() const override;

    Window* GetWindow() const;
    Key GetKey() const;
    int GetScancode() const;
    KeyState GetAction() const;
    int GetModifiers() const;

private:
    Window* m_window;
    Key m_key;
    int m_scancode;
    KeyState m_action;
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
    MouseButtonEvent(Window* window, MouseButton button, KeyState action, int modifiers);

    std::wstring GetName() const override;

    Window* GetWindow() const;
    MouseButton GetButton() const;
    KeyState GetAction() const;
    int GetModifiers() const;

private:
    Window* m_window;
    MouseButton m_button;
    KeyState m_action;
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
    virtual KeyState GetKey(Key key) const = 0;
    virtual KeyState GetMouseButton(MouseButton button) const = 0;

    /// @brief Return elpased time in seconds.
    virtual double GetTime() const = 0;

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
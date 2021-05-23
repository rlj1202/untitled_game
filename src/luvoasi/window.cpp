#include "luvoasi/window.h"

namespace Luvoasi {

KeyEvent::KeyEvent(Window* window, int key, int scancode, int action, int modifiers)
    : m_window(window), m_key(key), m_scancode(scancode), m_action(action), m_modifiers(modifiers) {}

std::wstring KeyEvent::GetName() const {
    return L"KeyEvent";
}

Window* KeyEvent::GetWindow() const {
    return m_window;
}

int KeyEvent::GetKey() const {
    return m_key;
}

int KeyEvent::GetScancode() const {
    return m_scancode;
}

int KeyEvent::GetAction() const {
    return m_action;
}

int KeyEvent::GetModifiers() const {
    return m_modifiers;
}

ScrollEvent::ScrollEvent(Window* window, double x_offset, double y_offset)
    : m_window(window), m_x_offset(x_offset), m_y_offset(y_offset) {}

std::wstring ScrollEvent::GetName() const {
    return L"ScrollEvent";
}

Window* ScrollEvent::GetWindow() const {
    return m_window;
}

double ScrollEvent::GetXOffset() const {
    return m_x_offset;
}

double ScrollEvent::GetYOffset() const {
    return m_y_offset;
}

MouseButtonEvent::MouseButtonEvent(Window* window, int button, int action, int modifiers)
    : m_window(window), m_button(button), m_action(action), m_mods(modifiers) {}

std::wstring MouseButtonEvent::GetName() const {
    return L"MouseButtonEvent";
}

Window* MouseButtonEvent::GetWindow() const {
    return m_window;
}

int MouseButtonEvent::GetButton() const {
    return m_button;
}

int MouseButtonEvent::GetAction() const {
    return m_action;
}

int MouseButtonEvent::GetModifiers() const {
    return m_mods;
}

CursorPosEvent::CursorPosEvent(Window* window, double x, double y)
    : m_window(window), m_x(x), m_y(y) {}

std::wstring CursorPosEvent::GetName() const {
    return L"CursorPosEvent";
}

Window* CursorPosEvent::GetWindow() const {
    return m_window;
}

double CursorPosEvent::GetX() const {
    return m_x;
}

double CursorPosEvent::GetY() const {
    return m_y;
}

}

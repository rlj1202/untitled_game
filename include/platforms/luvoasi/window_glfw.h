#ifndef H_LUVOASI_WINDOW_GLFW
#define H_LUVOASI_WINDOW_GLFW

#include "luvoasi/window.h"

#include <GLFW/glfw3.h>

namespace Luvoasi {

class GLFWWindow : public Window {
public:
    GLFWWindow(uint32_t width, uint32_t height, const char* title);
    GLFWWindow(const GLFWWindow& o) = delete;
    GLFWWindow(GLFWWindow&& o);
    virtual ~GLFWWindow();

    GLFWWindow& operator=(const GLFWWindow& o) = delete;
    GLFWWindow& operator=(GLFWWindow&& o) noexcept;

    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;

    void GetCursorPos(double* x, double* y) const override;
    uint32_t GetKey(uint32_t key) const override;
    uint32_t GetMouseButton(uint32_t button) const override;

    bool ShouldClose() const override;

    void SwapBuffers() const override;
    void PollEvents() const override;

    void AddKeyEventHandler(KeyEventHandler handler) override;
    void AddScrollEventHandler(ScrollEventHandler handler) override;
    void AddMouseButtonEventHandler(MouseButtonEventHandler handler) override;
    void AddCursorPosEventHandler(CursorPosEventHandler handler) override;

    void Bind() const override;

private:
    GLFWwindow* m_window;

    EventDispatcher<KeyEvent&> m_key_event;
    EventDispatcher<ScrollEvent&> m_scroll_event;
    EventDispatcher<MouseButtonEvent&> m_mouse_button_event;
    EventDispatcher<CursorPosEvent&> m_cursor_pos_event;

    static uint32_t s_window_count;

    static void KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
    static void ScrollEventCallback(GLFWwindow* window, double x, double y);
    static void MouseButtonEventCallback(GLFWwindow* window, int button, int action, int modifiers);
    static void CursorPosEventCallback(GLFWwindow* window, double x, double y);
};

}

#endif
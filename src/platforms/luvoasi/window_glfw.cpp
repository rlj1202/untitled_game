#include "platforms/luvoasi/window_glfw.h"

namespace Luvoasi {

uint32_t GLFWWindow::s_window_count = 0;

void GLFWWindow::KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int modifiers) {
    GLFWWindow* m_window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    KeyEvent event(m_window, (Key) key, scancode, (KeyState) action, modifiers);
    m_window->m_key_event(event);
}

void GLFWWindow::ScrollEventCallback(GLFWwindow* window, double x, double y) {
    GLFWWindow* m_window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    ScrollEvent event(m_window, x, y);
    m_window->m_scroll_event(event);
}

void GLFWWindow::MouseButtonEventCallback(GLFWwindow* window, int button, int action, int modifiers) {
    GLFWWindow* m_window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    MouseButtonEvent event(m_window, (MouseButton) button, (KeyState) action, modifiers);
    m_window->m_mouse_button_event(event);
}

void GLFWWindow::CursorPosEventCallback(GLFWwindow* window, double x, double y) {
    GLFWWindow* m_window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    CursorPosEvent event(m_window, x, y);
    m_window->m_cursor_pos_event(event);
}

GLFWWindow::GLFWWindow(uint32_t width, uint32_t height, const char* title) {
    if (!s_window_count) {
        if (!glfwInit()) {
            return;
        }
    }

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        return;
    }
    s_window_count++;

    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window, KeyEventCallback);
    glfwSetScrollCallback(m_window, ScrollEventCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonEventCallback);
    glfwSetCursorPosCallback(m_window, CursorPosEventCallback);
}

GLFWWindow::GLFWWindow(GLFWWindow&& o) {
    m_window = o.m_window;
    o.m_window = nullptr;
}

GLFWWindow::~GLFWWindow() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        s_window_count--;

        if (!s_window_count) {
            glfwTerminate();
        }
    }
}

GLFWWindow& GLFWWindow::operator=(GLFWWindow&& o) noexcept {
    if (this == &o) return *this;

    m_window = o.m_window;
    o.m_window = nullptr;

    return *this;
}

uint32_t GLFWWindow::GetWidth() const {
    int width;
    glfwGetWindowSize(m_window, &width, nullptr);

    return (uint32_t) width;
}

uint32_t GLFWWindow::GetHeight() const {
    int height;
    glfwGetWindowSize(m_window, nullptr, &height);

    return (uint32_t) height;
}

void GLFWWindow::GetCursorPos(double* x, double* y) const {
    glfwGetCursorPos(m_window, x, y);
}

KeyState GLFWWindow::GetKey(Key key) const {
    return (KeyState) glfwGetKey(m_window, (int32_t) key);
}

KeyState GLFWWindow::GetMouseButton(MouseButton button) const {
    return (KeyState) glfwGetMouseButton(m_window, (int32_t) button);
}

double GLFWWindow::GetTime() const {
    return glfwGetTime();
}

bool GLFWWindow::ShouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void GLFWWindow::SwapBuffers() const {
    glfwSwapBuffers(m_window);
}

void GLFWWindow::PollEvents() const {
    glfwPollEvents();
}

void GLFWWindow::AddKeyEventHandler(KeyEventHandler handler) {
    m_key_event.AddHandler(handler);
}

void GLFWWindow::AddScrollEventHandler(ScrollEventHandler handler) {
    m_scroll_event.AddHandler(handler);
}

void GLFWWindow::AddMouseButtonEventHandler(MouseButtonEventHandler handler) {
    m_mouse_button_event.AddHandler(handler);
}

void GLFWWindow::AddCursorPosEventHandler(CursorPosEventHandler handler) {
    m_cursor_pos_event.AddHandler(handler);
}

void GLFWWindow::Bind() const {
    glfwMakeContextCurrent(m_window);
}

GLFWwindow* GLFWWindow::GetRawPointer() const {
    return m_window;
}

std::unique_ptr<Window> CreateWindow(uint32_t width, uint32_t height, const char* title) {
    return std::move(
        std::make_unique<GLFWWindow>(width, height, title)
    );
}

}
#include "platforms/luvoasi/graphics/context_opengl.h"

#include "platforms/luvoasi/graphics/common_opengl.h"

#include <GLFW/glfw3.h>

namespace Luvoasi {

OpenGLContext::~OpenGLContext() {
    glfwTerminate();
}

void OpenGLContext::Init() {
    if (!glfwInit()) {
        return;
    }

    glfwCreateWindow(0, 0, nullptr, nullptr, nullptr);
}

void OpenGLContext::SwapBuffers() {
    glfwSwapBuffers(nullptr);
}

void OpenGLContext::PollEvents() {
    glfwPollEvents();
}

void OpenGLContext::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLContext::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

}
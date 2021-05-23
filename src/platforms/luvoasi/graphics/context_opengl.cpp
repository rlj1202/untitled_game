#include "platforms/luvoasi/graphics/context_opengl.h"

#include <GLES3/gl3.h>
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

}
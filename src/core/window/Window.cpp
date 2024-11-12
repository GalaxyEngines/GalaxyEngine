//
// Created by plaidmrdeer on 2024/11/12.
//
#define GLFW_INCLUDE_VULKAN
#include "Window.h"
#include <GLFW/glfw3.h>

GE::Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

GE::Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GE::Window::createWindow(const int width, const int height, const char *title) {
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

GLFWwindow *GE::Window::getWindow() const {
    return window;
}

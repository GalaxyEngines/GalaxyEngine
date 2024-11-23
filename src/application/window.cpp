//
// Created by plaidmrdeer on 2024/11/12.
//
#include <application/window.h>
ge::Window::Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

ge::Window::~Window()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void ge::Window::create_window(const int width_, const int height_, const char *title_)
{
    window_ = glfwCreateWindow(width_, height_, title_, nullptr, nullptr);
}

GLFWwindow *ge::Window::get_window() const
{
    return window_;
}

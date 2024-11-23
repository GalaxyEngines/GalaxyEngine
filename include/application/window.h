//
// Created by plaidmrdeer on 2024/11/12.
//
#pragma once
#define GLFW_INCLUDE_VULKAN
#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

namespace ge
{
    class Window
    {
    public:
        Window();
        ~Window();

         void create_window(int width_, int height_, const char* title_);

        [[nodiscard]] GLFWwindow* get_window() const;
    private:
        GLFWwindow* window_{};
    };
}


#endif

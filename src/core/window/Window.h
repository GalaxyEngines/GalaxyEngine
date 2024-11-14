//
// Created by plaidmrdeer on 2024/11/12.
//
#pragma once
#define GLFW_INCLUDE_VULKAN
#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

namespace GE {
    class Window {
    public:
        Window();
        ~Window();

         void createWindow(int width, int height, const char* title);

        [[nodiscard]] GLFWwindow* getWindow() const;
    private:
        GLFWwindow* window{};
    };
}


#endif

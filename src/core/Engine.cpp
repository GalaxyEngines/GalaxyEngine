//
// Created by plaidmrdeer on 2024/9/14.
//

#include <GLFW/glfw3.h>
#include <Engine.h>

void Engine::run()
{
    init();
    initVulkan();
    initWindow();
    loop();
    cleanup();
}

void Engine::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void initEngine() {

}

void Engine::initVulkan()
{

}

void Engine::initWindow()
{
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void Engine::loop()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void Engine::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}



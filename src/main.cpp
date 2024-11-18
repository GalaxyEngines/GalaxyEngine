#include <iostream>
#include "core/log/logger.h"
#include "core/VulkanBase.h"
#include "core/TaskScheduler.h"
#include "core/ModuleManager.h"
#include "core/MemoryManager.h"
#include "core/FiberManager.h"
#include "core/AsyncLoader.h"
#include <core/window/Window.h>

#include "../lib/GERender/src/graphics_base.h"

class GalaxyEngine
{
public:
    void run();
    GE::GraphicsBase get_graphics_base() const;
    GE::Logger logger{"logs"};
private:
    void initEngine();
    void initWindow();
    void initVulkan();
    void loop();
    void render();
    void cleanup();

    GE::Window window;
    GE::GraphicsBase graphics;
};

void GalaxyEngine::run()
{
    initEngine();
    initWindow();
    initVulkan();
    loop();
    render();
    cleanup();
}

GE::GraphicsBase GalaxyEngine::get_graphics_base() const
{
    return graphics;
}

void GalaxyEngine::initEngine()
{

    logger.log(GE::INFO, "初始化引擎...");
    logger.log(GE::INFO, "初始化引擎...完成");
}

void GalaxyEngine::initWindow()
{
    logger.log(GE::INFO, "初始化窗口...");

    window.createWindow(1920, 1080, "GalaxyEngine");
    logger.log(GE::INFO, "初始化窗口...完成");
}

void GalaxyEngine::initVulkan()
{
    logger.log(GE::INFO, "初始化Vulkan...");
    graphics.initializ();

    logger.log(GE::INFO, "初始化Vulkan...完成");
}

void GalaxyEngine::loop()
{
    logger.log(GE::INFO, "开始循环...");
    while (!glfwWindowShouldClose(window.getWindow()))
    {
        glfwPollEvents();
    }
    logger.log(GE::INFO, "循环...结束");
}

void GalaxyEngine::render()
{
    logger.log(GE::INFO, "开始渲染...");
    logger.log(GE::INFO, "渲染...完成");
}

void GalaxyEngine::cleanup()
{
    logger.log(GE::INFO, "清理资源...");
    logger.log(GE::INFO, "清理资源...完成");
}

int main()
{
    // GE::VulkanBase vulkanModule;
    // GE::TaskSchedulerModule taskScheduler;
    // GE::ModuleManager moduleManager;
    // GE::MemoryManager memoryManager;
    // GE::FiberManager fiberManager;
    // GE::AsyncLoader asyncLoader;

    // vulkanModule.initialize();
    // taskScheduler.initialize();
    // moduleManager.InitializeModules();
    // asyncLoader.initialize();

    // bool isRunning = true;
    // while (isRunning)
    // {
        // vulkanModule.update();
        // asyncLoader.update();
        // isRunning = false;
    // }

    // asyncLoader.shutdown();
    // fiberManager.shutdown();
    // moduleManager.CleanupModules();
    // taskScheduler.shutdown();
    // vulkanModule.shutdown();

    GalaxyEngine engine{};
    engine.run();

    std::cout << "所有模块已成功关闭，程序结束。" << std::endl;
    return 0;
}
#include <iostream>
#include "core/VulkanBase.h"
#include "core/TaskScheduler.h"
#include "core/ModuleManager.h"
#include "core/MemoryManager.h"
#include "core/FiberManager.h"
#include "core/AsyncLoader.h"

int main() {
    // Step 1: 创建并初始化所有模块
    GE::VulkanBase vulkanModule;
    GE::TaskScheduler taskScheduler;
    GE::ModuleManager moduleManager;
    GE::MemoryManager memoryManager;
    GE::FiberManager fiberManager;
    GE::AsyncLoader asyncLoader;

    // 初始化 Vulkan 模块
    vulkanModule.initialize();

    // 初始化任务调度器
    taskScheduler.initialize();

    // 初始化模块管理器
    moduleManager.initialize();

    // 初始化内存管理器
    memoryManager.initialize();

    // 初始化 Fiber 管理器
    fiberManager.initialize();

    // 初始化异步加载器
    asyncLoader.initialize();

    // Step 2: 主循环
    bool isRunning = true;
    while (isRunning) {
        // 更新 Vulkan 模块
        vulkanModule.update();

        // 更新任务调度器
        taskScheduler.update();

        // 更新模块管理器
        moduleManager.update();

        // 更新内存管理器
        memoryManager.update();

        // 更新 Fiber 管理器
        fiberManager.update();

        // 更新异步加载器
        asyncLoader.update();

        // 假设存在某种退出条件，在此例中直接退出循环
        isRunning = false;  // 可以根据需要调整为实际退出条件
    }

    // Step 3: 清理资源
    asyncLoader.shutdown();
    fiberManager.shutdown();
    memoryManager.shutdown();
    moduleManager.shutdown();
    taskScheduler.shutdown();
    vulkanModule.shutdown();

    std::cout << "所有模块已成功关闭，程序结束。" << std::endl;
    return 0;
}

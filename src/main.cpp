#include <iostream>
#include "core/VulkanBase.h"
#include "core/TaskScheduler.h"
#include "core/ModuleManager.h"
#include "core/MemoryManager.h"
#include "core/FiberManager.h"
#include "core/AsyncLoader.h"
#include "ui/a.h"  // UIInterface 的头文件

int main() {
    // Step 1: 创建并初始化所有模块
    MyEngine::VulkanBase VulkanBase;
    TaskSchedulerModule taskScheduler;
    ModuleManager moduleManager;
    MyEngine::MemoryManager memoryManager;
    MyEngine::AsyncLoader asyncLoader;

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

    // Step 2: 初始化 UI 界面 (从 a.h 引入的 UI 界面)
    UIInterface uiInterface;  // 从 a.h 中的类
    uiInterface.initialize(vulkanModule);  // UI 依赖 Vulkan 进行渲染

    // Step 3: 主循环
    bool isRunning = true;
    while (isRunning) {
        // 更新 Vulkan 模块
        vulkanModule.update();

        // 更新任务调度器
        taskScheduler.update();

        // 更新 UI 界面
        uiInterface.update();

        // 用户事件处理逻辑（例如关闭窗口）
        isRunning = !uiInterface.shouldClose();  // 检查是否退出主循环
    }

    // Step 4: 清理资源
    uiInterface.shutdown();
    asyncLoader.shutdown();
    fiberManager.shutdown();
    memoryManager.shutdown();
    moduleManager.shutdown();
    taskScheduler.shutdown();
    vulkanModule.shutdown();

    std::cout << "所有模块已成功关闭，程序结束。" << std::endl;
    return 0;
}

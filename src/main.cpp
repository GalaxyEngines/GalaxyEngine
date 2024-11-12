#include <iostream>
#include "core/VulkanBase.h"
#include "core/TaskScheduler.h"
#include "core/ModuleManager.h"
#include "core/MemoryManager.h"
#include "core/FiberManager.h"
#include "core/AsyncLoader.h"

#include <render.h>

#include "core/log/Logger.h"
int main() {
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
    // while (isRunning) {
        // vulkanModule.update();
        // asyncLoader.update();
        // isRunning = false;
    // }

    // asyncLoader.shutdown();
    // fiberManager.shutdown();
    // moduleManager.CleanupModules();
    // taskScheduler.shutdown();
    // vulkanModule.shutdown();
    GE::Logger logger{"logs"};

    logger.log(GE::LogLevel::INFO, "Hello World!");

    a();

    std::cout << "所有模块已成功关闭，程序结束。" << std::endl;
    return 0;
}

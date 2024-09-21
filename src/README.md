模块开发结构
ExampleModule.h

#pragma once

#include "ModuleInterface.h"
#include <iostream>

class ExampleModule : public ModuleInterface {
public:
    void Initialize() override {
        std::cout << "ExampleModule 初始化。" << std::endl;
    }

    void Cleanup() override {
        std::cout << "ExampleModule 清理。" << std::endl;
    }

    void OnEvent(const std::string& event) override {
        std::cout << "ExampleModule 收到事件: " << event << std::endl;
    }

    void LoadConfig(const std::string& configFile) override {
        // 从配置文件加载设置
        std::cout << "从 " << configFile << " 加载配置。" << std::endl;
    }
};
主程序修改

#include "ModuleManager.h"
#include "ExampleModule.h"
#include "VulkanMemoryManagerModule.h"
#include "TaskScheduler.h"
..........
//  Vulkan 设备和物理设备
VkDevice device = ...;
VkPhysicalDevice physicalDevice = ...;
TaskScheduler* taskScheduler = new TaskScheduler();

int main() {
    ModuleManager moduleManager;

    // 注册模块
    moduleManager.RegisterModule("ExampleModule", std::make_unique<ExampleModule>());
    moduleManager.RegisterModule("VulkanMemoryManager", std::make_unique<VulkanMemoryManagerModule>(device, physicalDevice, taskScheduler));

    // 添加依赖关系
    moduleManager.AddDependency("VulkanMemoryManager", "ExampleModule");

    // 初始化所有模块
    moduleManager.InitializeModules();

    // 发送事件
    moduleManager.OnEvent("事件");

    // 动态加载模块
    moduleManager.LoadModuleFromFile("path/to/dynamic/module.so");

    // 清理模块
    moduleManager.CleanupModules();
    moduleManager.UnregisterModule("VulkanMemoryManager");
    moduleManager.UnregisterModule("ExampleModule");

    delete taskScheduler;

    return 0;
}

#ifndef VULKAN_BASE_H
#define VULKAN_BASE_H

#include "ModuleInterface.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <optional>

namespace MyEngine {

    class VulkanBaseModule : public ModuleInterface {
    public:
        VulkanBaseModule();
        virtual ~VulkanBaseModule();

        // 初始化 Vulkan
        void initialize() override;

        // 清理 Vulkan
        void shutdown() override;

        // 处理事件
        void onEvent(const std::string& event) override;

        // 处理任务
        void processTask(const Task& task) override;

        // 设置窗口句柄，用于与窗口系统集成
        void setWindowHandle(void* windowHandle);

        // 获取可用的 GPU 列表
        std::vector<std::string> getAvailableGPUs();

    private:
        // Vulkan 实例和设备
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        // Vulkan 逻辑
        void initVulkan();
        void cleanupVulkan();
        void createInstance();
        void createDevice();
        void renderFrame();

        // 检查 Vulkan 设备是否合适
        bool isDeviceSuitable(VkPhysicalDevice device);

        // 窗口句柄
        void* windowHandle;
    };

}

#endif // VULKAN_BASE_H

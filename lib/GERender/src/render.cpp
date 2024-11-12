#include "render.h"

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>

void a() {
    VkResult result = VK_SUCCESS;

    // 查询可用的 Vulkan 实例层
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    std::cout << "Available Vulkan Instance Layers:" << std::endl;
    for (const auto& layer : layerProperties) {
        std::cout << "  " << layer.layerName << ": " << layer.description << std::endl;
    }

    // 创建 Vulkan 实例
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return;
    }

    std::cout << "Vulkan instance created successfully." << std::endl;

    // 查询可用的物理设备
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        std::cerr << "Failed to find GPUs with Vulkan support!" << std::endl;
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::cout << "Available Vulkan Physical Devices:" << std::endl;
    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "  Device Name: " << deviceProperties.deviceName << std::endl;
        std::cout << "  Device Type: " << deviceProperties.deviceType << std::endl;
        std::cout << "  API Version: "
                  << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
                  << VK_VERSION_MINOR(deviceProperties.apiVersion) << "."
                  << VK_VERSION_PATCH(deviceProperties.apiVersion) << std::endl;
    }

    // 清理 Vulkan 实例
    vkDestroyInstance(instance, nullptr);
    std::cout << "Vulkan instance destroyed successfully." << std::endl;
}

// #ifndef VULKAN_BASE_H
// #define VULKAN_BASE_H
//
// #include "ModuleInterface.h"
// #include <vulkan/vulkan.h>
// #include <GLFW/glfw3.h>  // 需要GLFW库用于窗口系统集成
// #include <vector>
// #include <string>
// #include <set>
// #include <optional>
//
// namespace GE {
//
//     class VulkanBase : public ModuleInterface {
//     public:
//         VulkanBase();
//         virtual ~VulkanBase();
//
//         // 初始化 Vulkan
//         void initialize() override;
//
//         // 清理 Vulkan
//         void shutdown() override;
//
//         // 处理事件
//         void onEvent(const std::string& event) override;
//
//         // 处理任务
//         void processTask(const Task& task) override;
//
//         // 设置窗口句柄，用于与窗口系统集成
//         void setWindowHandle(void* windowHandle);
//
//         // 获取可用的 GPU 列表
//         std::vector<std::string> getAvailableGPUs();
//
//         // 渲染一帧
//         void renderFrame();
//
//         // 获取渲染的图像
//         void getRenderedImage();
//
//     private:
//         // Vulkan 实例和设备
//         VkInstance instance;
//         VkPhysicalDevice physicalDevice;
//         VkDevice device;
//         VkQueue graphicsQueue;
//         VkSurfaceKHR surface;
//         void* windowHandle;
//
//         // Vulkan 逻辑
//         void initVulkan();
//         void cleanupVulkan();
//         void createInstance();
//         void createSurface();
//         void pickPhysicalDevice();
//         void createLogicalDevice();
//
//         // 检查 Vulkan 设备是否合适
//         bool isDeviceSuitable(VkPhysicalDevice device);
//
//         // 队列家族索引
//         struct QueueFamilyIndices {
//             std::optional<uint32_t> graphicsFamily;
//             std::optional<uint32_t> presentFamily;
//
//             bool isComplete() const {
//                 return graphicsFamily.has_value() && presentFamily.has_value();
//             }
//         };
//
//         QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
//         // Vulkan 扩展
//         std::vector<const char*> getRequiredExtensions();
//         bool checkDeviceExtensionSupport(VkPhysicalDevice device);
//
//         // 设备扩展
//         std::vector<const char*> deviceExtensions = {
//             VK_KHR_SWAPCHAIN_EXTENSION_NAME
//         };
//
//         //jiegouti交换链
//         struct SwapChainSupportDetails {
//             VkSurfaceCapabilitiesKHR capabilities;
//             std::vector<VkSurfaceFormatKHR> formats;
//             std::vector<VkPresentModeKHR> presentModes;
//         };
//
//         SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
//     };
//
// }
//
// #endif // VULKAN_BASE_H

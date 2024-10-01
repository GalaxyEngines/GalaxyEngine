#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>
#include <windows.h>

// 定义游戏对象
struct GameObject {
    char name[32] = "游戏";
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
};

// 全局状态变量
GameObject currentObject;
bool showSceneWindow = true;
bool showHierarchyWindow = true;
bool showInspectorWindow = true;
bool showConsoleWindow = true;
bool showResourceBrowserWindow = true;
bool showPerformanceWindow = true;
std::string currentFilePath;
// 性能数据
float cpuUsage = 0.0f;
float gpuUsage = 0.0f;
int fps = 0;

// 获取当前时间
std::string GetCurrentTime() {
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}

// 使用系统 API 获取 CPU 性能数据
void UpdateCPUUsage() {
    static FILETIME prevIdleTime, prevKernelTime, prevUserTime;
    FILETIME idleTime, kernelTime, userTime;

    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULONGLONG idleDiff = (static_cast<ULONGLONG>(idleTime.dwLowDateTime) |
                               (static_cast<ULONGLONG>(idleTime.dwHighDateTime) << 32)) -
                              (static_cast<ULONGLONG>(prevIdleTime.dwLowDateTime) |
                               (static_cast<ULONGLONG>(prevIdleTime.dwHighDateTime) << 32));
        ULONGLONG kernelDiff = (static_cast<ULONGLONG>(kernelTime.dwLowDateTime) |
                                (static_cast<ULONGLONG>(kernelTime.dwHighDateTime) << 32)) -
                               (static_cast<ULONGLONG>(prevKernelTime.dwLowDateTime) |
                                (static_cast<ULONGLONG>(prevKernelTime.dwHighDateTime) << 32));
        ULONGLONG userDiff = (static_cast<ULONGLONG>(userTime.dwLowDateTime) |
                              (static_cast<ULONGLONG>(userTime.dwHighDateTime) << 32)) -
                             (static_cast<ULONGLONG>(prevUserTime.dwLowDateTime) |
                              (static_cast<ULONGLONG>(prevUserTime.dwHighDateTime) << 32));

        ULONGLONG totalDiff = kernelDiff + userDiff;
        cpuUsage = (totalDiff - idleDiff) / static_cast<float>(totalDiff);

        prevIdleTime = idleTime;
        prevKernelTime = kernelTime;
        prevUserTime = userTime;
    }
}

// 使用 Vulkan API 获取 GPU 性能数据
void UpdateGPUUsage(VkDevice device, VkQueue queue) {
    VkQueryPoolCreateInfo queryPoolInfo = {};
    queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    queryPoolInfo.queryCount = 2;

    VkQueryPool queryPool;
    if (vkCreateQueryPool(device, &queryPoolInfo, nullptr, &queryPool) != VK_SUCCESS) {
        throw std::runtime_error("你的query pool炸了!");
    }

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool, 0);
    vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPool, 1);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    VkQueryResultFlags flags = VK_QUERY_RESULT_WAIT_BIT | VK_QUERY_RESULT_64_BIT;
    uint64_t timestamps[2];
    vkGetQueryPoolResults(device, queryPool, 0, 2, sizeof(timestamps), timestamps, sizeof(uint64_t), flags);

    // 计算 GPU 使用率
    gpuUsage = static_cast<float>(timestamps[1] - timestamps[0]) / 1000000.0f; // 示例值，需要根据实际情况调整

    vkDestroyQueryPool(device, queryPool, nullptr);
}


// 更新性能数据
void UpdatePerformanceData(VkDevice device, VkQueue queue) {
    static auto lastTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - lastTime;

    UpdateCPUUsage();
    UpdateGPUUsage(device, queue);

    // 计算 FPS
    static int frameCount = 0;
    static float elapsedSeconds = 0.0f;
    frameCount++;
    elapsedSeconds += elapsedTime.count();
    if (elapsedSeconds >= 1.0f) {
        fps = frameCount;
        frameCount = 0;
        elapsedSeconds = 0.0f;
    }

    lastTime = currentTime;
}

// 文件操作功能
void SaveFile(const std::string& filePath) {
    try {
        std::ofstream outFile(filePath);
        if (outFile) {
            outFile << "对象名称: " << currentObject.name << "\n";
            outFile << "位置: " << currentObject.position[0] << ", "
                    << currentObject.position[1] << ", "
                    << currentObject.position[2] << "\n";
            outFile << "旋转: " << currentObject.rotation[0] << ", "
                    << currentObject.rotation[1] << ", "
                    << currentObject.rotation[2] << "\n";
            outFile << "缩放: " << currentObject.scale[0] << ", "
                    << currentObject.scale[1] << ", "
                    << currentObject.scale[2] << "\n";
            std::cout << "文件已保存: " << filePath << std::endl;
        } else {
            std::cerr << "保存文件失败！" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "保存文件时发生错误: " << e.what() << std::endl;
    }
}

void OpenFile(const std::string& filePath) {
    try {
        std::ifstream inFile(filePath);
        if (inFile) {
            std::string line;
            while (std::getline(inFile, line)) {
                std::cout << line << std::endl;
            }
            currentFilePath = filePath;
            std::cout << "文件已打开: " << filePath << std::endl;
        } else {
            std::cerr << "无法打开文件！" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "打开文件时发生错误: " << e.what() << std::endl;
    }
}

// 渲染性能监控窗口
void RenderPerformanceWindow() {
    if (!showPerformanceWindow) return; 
    ImGui::Begin("性能监控", &showPerformanceWindow);
    ImGui::Text("性能数据:");
    ImGui::Text("CPU 使用率: %.1f%%", cpuUsage * 100.0f);
    ImGui::Text("GPU 使用率: %.1f%%", gpuUsage * 100.0f);
    ImGui::Text("FPS: %d", fps);
    ImGui::End();
}

// 创建主菜单栏
void RenderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("文件")) {
            if (ImGui::MenuItem("新建")) {
                currentFilePath.clear();
                std::cout << "新建项目" << std::endl;
            }
            if (ImGui::MenuItem("打开", "Ctrl+O")) {
                OpenFile("项目文件.txt");
            }
            if (ImGui::MenuItem("保存", "Ctrl+S")) {
                SaveFile(currentFilePath.empty() ? "默认项目文件.txt" : currentFilePath);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("窗口")) {
            ImGui::MenuItem("场景", nullptr, &showSceneWindow);
            ImGui::MenuItem("层次结构", nullptr, &showHierarchyWindow);
            ImGui::MenuItem("属性检查器", nullptr, &showInspectorWindow);
            ImGui::MenuItem("控制台", nullptr, &showConsoleWindow);
            ImGui::MenuItem("资源浏览器", nullptr, &showResourceBrowserWindow);
            ImGui::MenuItem("性能监控", nullptr, &showPerformanceWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// 初始化 Vulkan
void SetupVulkan(ImGui_ImplVulkan_InitInfo& vulkanInitInfo, VkInstance& instance, VkDevice& device, VkPhysicalDevice& physicalDevice, VkQueue& graphicsQueue, VkRenderPass& renderPass, VkDescriptorPool& descriptorPool) {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ImGui Vulkan Example";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("你的Vulkan instance炸了!");
    }

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    physicalDevice = physicalDevices[0]; // 选择第一个设备

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0; // 需要设置队列族索引
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("创建vulkandevice失败!");
    }

    vkGetDeviceQueue(device, 0, 0, &graphicsQueue);

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM; // 颜色附件格式
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("你的render pass炸了!");
    }
    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        // 省略其他池大小设置
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = 1000;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("你的descriptor pool炸了!");
    }

    vulkanInitInfo.Instance = instance;
    vulkanInitInfo.Device = device;
    vulkanInitInfo.PhysicalDevice = physicalDevice;
    vulkanInitInfo.QueueFamily = 0;  // 需要设置队列族
    vulkanInitInfo.Queue = graphicsQueue;
    vulkanInitInfo.PipelineCache = VK_NULL_HANDLE;
    vulkanInitInfo.DescriptorPool = descriptorPool;
    vulkanInitInfo.Allocator = nullptr;
    vulkanInitInfo.CheckVkResultFn = nullptr;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW 初始化失败！" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "游戏引擎", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "窗口创建失败！" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // 开启垂直同步

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);

    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkRenderPass renderPass;
    VkDescriptorPool descriptorPool;

    ImGui_ImplVulkan_InitInfo vulkanInitInfo = {};
    SetupVulkan(vulkanInitInfo, instance, device, physicalDevice, graphicsQueue, renderPass, descriptorPool);
    ImGui_ImplVulkan_Init(&vulkanInitInfo, renderPass);

    // 创建命令缓冲区
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0; // 需要设置实际的队列族
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("命令缓冲区创建失败!");
    }

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("你的buffer炸了!");
    }

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        UpdatePerformanceData(device, graphicsQueue);

        // 开始新的 ImGui 帧
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 渲染 UI 界面
        RenderMainMenuBar();
        RenderPerformanceWindow();

        // 渲染 ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        VkClearValue clearColor = {};
        clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = VK_NULL_HANDLE;  // 需要设置framebuffer
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = { static_cast<uint32_t>(display_w), static_cast<uint32_t>(display_h) };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkBeginCommandBuffer(commandBuffer, nullptr);
        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
        vkCmdEndRenderPass(commandBuffer);
        vkEndCommandBuffer(commandBuffer);

        // 提交命令缓冲区
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = nullptr;  // 需要设置 swapchain
        presentInfo.pImageIndices = nullptr;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(graphicsQueue, &presentInfo);

        glfwSwapBuffers(window);
    }

    // 清理
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

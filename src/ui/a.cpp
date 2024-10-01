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
#include "TaskSchedulerModule.h"  // 引入任务调度模块
#include "ModuleInterface.h"

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
void UpdateGPUUsage(VkDevice device, VkQueue queue, VkCommandBuffer commandBuffer) {
    VkQueryPoolCreateInfo queryPoolInfo = {};
    queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    queryPoolInfo.queryCount = 2;

    VkQueryPool queryPool;
    if (vkCreateQueryPool(device, &queryPoolInfo, nullptr, &queryPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create query pool!");
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

    gpuUsage = static_cast<float>(timestamps[1] - timestamps[0]) / 1000000.0f; // 示例值

    vkDestroyQueryPool(device, queryPool, nullptr);
}

// 更新性能数据，使用调度器处理
void UpdatePerformanceData(TaskSchedulerModule& taskScheduler, VkDevice device, VkQueue queue, VkCommandBuffer commandBuffer) {
    taskScheduler.ScheduleTask([device, queue, commandBuffer]() {
        UpdateCPUUsage();
        UpdateGPUUsage(device, queue, commandBuffer);
    });
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
            if (ImGui::MenuItem("保存", "Ctrl+S")) {
                SaveFile(currentFilePath.empty() ? "默认项目文件.txt" : currentFilePath);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

int main() {
    // 初始化调度器
    TaskSchedulerModule taskScheduler;
    taskScheduler.Initialize();

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
    VkCommandBuffer commandBuffer;
    CreateCommandBuffer(device, commandPool, commandBuffer);  // 创建命令缓冲区

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        UpdatePerformanceData(taskScheduler, device, graphicsQueue, commandBuffer);  // 使用调度器更新性能数据

        // 开始新的 ImGui 帧
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderMainMenuBar();
        RenderPerformanceWindow();

        // 渲染 ImGui
        ImGui::Render();
        RenderImGui(device, commandBuffer, graphicsQueue, renderPass, window);  // 渲染 ImGui 并提交命令缓冲区

        glfwSwapBuffers(window);
    }

    // 清理
    taskScheduler.Cleanup();
    CleanupVulkan(device, commandPool, descriptorPool, renderPass, instance);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

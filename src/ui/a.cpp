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
    char name[32] = "游戏对象";
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
std::string currentFilePath = "default.txt";
float cpuUsage = 0.0f;
float gpuUsage = 0.0f;
int fps = 0;

// 日志内容
std::vector<std::string> logMessages;

// 自定义 UI 样式和颜色
void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f; // 让按钮和框架圆润
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f); // 暗黑模式背景
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.3f, 0.4f, 1.0f); // 关键按钮使用亮色
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.45f, 0.6f, 1.0f); // 按钮悬停时颜色
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.3f, 0.5f, 1.0f); // 按钮激活时颜色
    style.Colors[ImGuiCol_Text] = ImVec4(0.8f, 0.8f, 0.83f, 1.0f); // 文本颜色为柔和的白色
}

// 更新性能数据
void UpdatePerformanceData(TaskSchedulerModule& taskScheduler, VkDevice device, VkQueue queue, VkCommandBuffer commandBuffer) {
    taskScheduler.ScheduleTask([device, queue, commandBuffer]() {
        UpdateCPUUsage();
        UpdateGPUUsage(device, queue, commandBuffer);
    });
}

// 文件保存
void SaveFile(const std::string& filePath) {
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
        logMessages.push_back("文件已保存: " + filePath);
    } else {
        std::cerr << "保存文件失败！" << std::endl;
        logMessages.push_back("保存文件失败！");
    }
}

// 渲染场景窗口
void RenderSceneWindow() {
    if (!showSceneWindow) return;
    ImGui::Begin("场景视图", &showSceneWindow);
    ImGui::Text("场景视图在这里");
    // TODO: 在这里实现场景的实时3D渲染
    ImGui::End();
}

// 渲染层次结构窗口
void RenderHierarchyWindow() {
    if (!showHierarchyWindow) return;
    ImGui::Begin("层次结构", &showHierarchyWindow);
    if (ImGui::TreeNode("游戏对象")) {
        ImGui::Text("名称: %s", currentObject.name);
        ImGui::TreePop();
    }
    ImGui::End();
}

// 渲染属性检查器窗口
void RenderInspectorWindow() {
    if (!showInspectorWindow) return;
    ImGui::Begin("属性检查器", &showInspectorWindow);
    ImGui::InputText("名称", currentObject.name, IM_ARRAYSIZE(currentObject.name));
    ImGui::DragFloat3("位置", currentObject.position, 0.1f);
    ImGui::DragFloat3("旋转", currentObject.rotation, 0.1f);
    ImGui::DragFloat3("缩放", currentObject.scale, 0.1f);
    ImGui::End();
}

// 渲染控制台窗口
void RenderConsoleWindow() {
    if (!showConsoleWindow) return;
    ImGui::Begin("控制台", &showConsoleWindow);
    for (const auto& msg : logMessages) {
        ImGui::TextUnformatted(msg.c_str());
    }
    ImGui::End();
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
                SaveFile(currentFilePath);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("视图")) {
            ImGui::MenuItem("场景", nullptr, &showSceneWindow);
            ImGui::MenuItem("层次结构", nullptr, &showHierarchyWindow);
            ImGui::MenuItem("属性检查器", nullptr, &showInspectorWindow);
            ImGui::MenuItem("控制台", nullptr, &showConsoleWindow);
            ImGui::MenuItem("性能监控", nullptr, &showPerformanceWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// 主函数
int main() {
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
    SetupImGuiStyle();  // 应用自定义的UI样式
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
    CreateCommandBuffer(device, commandPool, commandBuffer);

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        UpdatePerformanceData(taskScheduler, device, graphicsQueue, commandBuffer);

        // 开始新的 ImGui 帧
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderMainMenuBar();
        RenderSceneWindow();
        RenderHierarchyWindow();
        RenderInspectorWindow();
        RenderConsoleWindow();
        RenderPerformanceWindow();

        // 渲染 ImGui
        ImGui::Render();
        RenderImGui(device, commandBuffer, graphicsQueue, renderPass, window);

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

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
#include "TaskSchedulerModule.h"
#include "VulkanBase.h"
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

// Vulkan 渲染相关全局变量
VulkanBase* vulkanBase;
VkCommandBuffer sceneCommandBuffer;

// 日志内容
std::vector<std::string> logMessages;

// 设定UI字体和风格
void SetupImGuiFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 16.0f);  // 使用 Roboto 字体
}

void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);  // 更柔和的深色背景
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.35f, 0.45f, 1.0f);  // 亮色按钮
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.45f, 0.6f, 1.0f);  // 悬停颜色
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.4f, 0.55f, 1.0f);  // 按钮激活时颜色
    style.Colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.88f, 1.0f);  // 柔和的白色文本
    style.WindowRounding = 6.0f;  // 圆角边框
}

// 实现窗口拖拽、调整大小
void BeginDocking() {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

// 渲染场景窗口 (显示 Vulkan 渲染的结果)
void RenderSceneWindow() {
    if (!showSceneWindow) return;
    
    ImGui::Begin("场景视图", &showSceneWindow);

    // 自适应调整场景窗口大小
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    
    // 显示 Vulkan 渲染的图像
    ImGui::Image((void*)(intptr_t)vulkanBase->GetRenderedImage(), windowSize);

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
    SetupImGuiFonts();  // 设置自定义字体
    SetupImGuiStyle();  // 设置UI样式
    ImGui_ImplGlfw_InitForVulkan(window, true);

    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkRenderPass renderPass;
    VkDescriptorPool descriptorPool;

    // 初始化 Vulkan
    vulkanBase = new VulkanBase();
    vulkanBase->Initialize(window, instance, device, physicalDevice, graphicsQueue, renderPass, descriptorPool);
    sceneCommandBuffer = vulkanBase->CreateCommandBuffer();

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        UpdatePerformanceData(taskScheduler, device, graphicsQueue, sceneCommandBuffer);

        // 开始新的 ImGui 帧
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        BeginDocking();  // 允许窗口拖拽和布局调整
        RenderMainMenuBar();
        RenderSceneWindow();  // 显示 Vulkan 场景渲染
        RenderHierarchyWindow();
        RenderInspectorWindow();
        RenderConsoleWindow();
        RenderPerformanceWindow();

        // 渲染 ImGui 和 Vulkan 场景
        ImGui::Render();
        vulkanBase->RenderScene(sceneCommandBuffer);  // 渲染场景
        RenderImGui(device, sceneCommandBuffer, graphicsQueue, renderPass, window);

        glfwSwapBuffers(window);
    }

    // 清理
    taskScheduler.Cleanup();
    vulkanBase->Cleanup();
    CleanupVulkan(device, sceneCommandBuffer, descriptorPool, renderPass, instance);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

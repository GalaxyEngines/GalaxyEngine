#include "a.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>
#include "TaskScheduler.h"
#include "VulkanBase.h"
#include "ModuleInterface.h"

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
MyEngine::VulkanBase* vulkanBase;  // VulkanBase 使用全局指针
VkCommandBuffer sceneCommandBuffer;
std::vector<std::string> logMessages;

void UIInterface::initialize(MyEngine::VulkanBase& vulkanModule) {
    vulkanBase = &vulkanModule;  // 使用传入的 Vulkan 模块
}

void UIInterface::update() {
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
    vulkanBase->renderFrame();  // 渲染场景
}

void UIInterface::shutdown() {
    // 清理 ImGui 和 Vulkan 相关资源
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool UIInterface::shouldClose() {
    // 用于检查窗口是否应关闭
    return glfwWindowShouldClose(glfwGetCurrentContext());
}

// 设置自定义字体
void UIInterface::SetupImGuiFonts() {
    ImGuiIO& file_io = ImGui::GetIO();
    file_io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 16.0f);  // 使用 Roboto 字体
}

// 设置UI样式
void UIInterface::SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);  // 更柔和的深色背景
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.35f, 0.45f, 1.0f);  // 亮色按钮
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.45f, 0.6f, 1.0f);  // 悬停颜色
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.4f, 0.55f, 1.0f);  // 按钮激活时颜色
    style.Colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.88f, 1.0f);  // 柔和的白色文本
    style.WindowRounding = 6.0f;  // 圆角边框
}

// 渲染函数
void UIInterface::RenderSceneWindow() {
    if (!showSceneWindow) return;

    ImGui::Begin("场景视图", &showSceneWindow);
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(intptr_t)vulkanBase->getRenderedImage(), windowSize);  // 显示 Vulkan 渲染的图像
    ImGui::End();
}

void UIInterface::RenderHierarchyWindow() {
    if (!showHierarchyWindow) return;
    ImGui::Begin("层次结构", &showHierarchyWindow);
    if (ImGui::TreeNode("游戏对象")) {
        ImGui::Text("名称: %s", currentObject.name);
        ImGui::TreePop();
    }
    ImGui::End();
}

void UIInterface::RenderInspectorWindow() {
    if (!showInspectorWindow) return;
    ImGui::Begin("属性检查器", &showInspectorWindow);
    ImGui::InputText("名称", currentObject.name, IM_ARRAYSIZE(currentObject.name));
    ImGui::DragFloat3("位置", currentObject.position, 0.1f);
    ImGui::DragFloat3("旋转", currentObject.rotation, 0.1f);
    ImGui::DragFloat3("缩放", currentObject.scale, 0.1f);
    ImGui::End();
}

void UIInterface::RenderConsoleWindow() {
    if (!showConsoleWindow) return;
    ImGui::Begin("控制台", &showConsoleWindow);
    for (const auto& msg : logMessages) {
        ImGui::TextUnformatted(msg.c_str());
    }
    ImGui::End();
}

void UIInterface::RenderPerformanceWindow() {
    if (!showPerformanceWindow) return;
    ImGui::Begin("性能监控", &showPerformanceWindow);
    ImGui::Text("性能数据:");
    ImGui::Text("CPU 使用率: %.1f%%", cpuUsage * 100.0f);
    ImGui::Text("GPU 使用率: %.1f%%", gpuUsage * 100.0f);
    ImGui::Text("FPS: %d", fps);
    ImGui::End();
}

void UIInterface::RenderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("文件")) {
            if (ImGui::MenuItem("保存", "Ctrl+S")) {
                // 保存文件逻辑
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


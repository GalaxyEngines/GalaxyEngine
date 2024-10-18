#ifndef UI_A_H
#define UI_A_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

// Forward declare VulkanBase (since we don't need full definition here)
namespace MyEngine {
    class VulkanBase;
}

// 游戏对象的结构体
struct GameObject {
    char name[32] = "游戏对象";
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
};

// 全局状态变量
extern GameObject currentObject;
extern bool showSceneWindow;
extern bool showHierarchyWindow;
extern bool showInspectorWindow;
extern bool showConsoleWindow;
extern bool showResourceBrowserWindow;
extern bool showPerformanceWindow;
extern std::string currentFilePath;
extern float cpuUsage;
extern float gpuUsage;
extern int fps;
extern MyEngine::VulkanBase* vulkanBase;  // VulkanBase is used here as a pointer
extern VkCommandBuffer sceneCommandBuffer;
extern std::vector<std::string> logMessages;

// UI 界面接口类定义
class UIInterface {
public:
    void initialize(MyEngine::VulkanBase& vulkanModule);  // VulkanBase passed by reference
    void update();
    void shutdown();
    bool shouldClose();

private:
    void SetupImGuiFonts();
    void SetupImGuiStyle();
    void BeginDocking();
    void RenderSceneWindow();
    void RenderHierarchyWindow();
    void RenderInspectorWindow();
    void RenderConsoleWindow();
    void RenderPerformanceWindow();
    void RenderMainMenuBar();
};

#endif // UI_A_H

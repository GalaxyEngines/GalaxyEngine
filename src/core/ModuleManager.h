#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_set>
#include "ModuleInterface.h"

class ModuleManager {
public:
    ModuleManager() = default;
    ~ModuleManager();

    // 注册模块
    void RegisterModule(const std::string& name, std::unique_ptr<ModuleInterface> module);

    // 卸载模块
    void UnregisterModule(const std::string& name);

    // 初始化所有模块
    void InitializeModules();

    // 清理所有模块
    void CleanupModules();

    // 处理事件
    void OnEvent(const std::string& event);

    // 添加和移除模块依赖
    void AddDependency(const std::string& module, const std::string& dependency);
    void RemoveDependency(const std::string& module, const std::string& dependency);

    // 加载和卸载动态模块
    void LoadModuleFromFile(const std::string& filePath, const std::string& moduleName);
    void UnloadModule(const std::string& name);

private:
    std::map<std::string, std::unique_ptr<ModuleInterface>> modules;  // 模块集合
    std::map<std::string, std::vector<std::string>> dependencyGraph;  // 依赖关系图
    std::map<std::string, void*> loadedModules;  // 动态加载的模块

    std::mutex moduleMutex;  // 线程安全

    // 拓扑排序，返回拓扑排序结果
    bool TopologicalSort(std::vector<std::string>& sortedModules);
};

#endif // MODULE_MANAGER_H

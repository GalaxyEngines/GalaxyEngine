// // ModuleManager.h
// //国庆节修改
//
// #ifndef MODULE_MANAGER_H
// #define MODULE_MANAGER_H
//
// #include "ModuleInterface.h"
// #include <string>
// #include <unordered_map>
// #include <vector>
// #include <memory>
// #include <mutex>
// #include <filesystem>
//
// #ifdef _WIN32
// #include <Windows.h>
// #else
// #include <dlfcn.h>
// #endif
//
// class ModuleManager {
// public:
//     ModuleManager() = default;
//     ~ModuleManager();
//
//     // 禁止拷贝和赋值
//     ModuleManager(const ModuleManager&) = delete;
//     ModuleManager& operator=(const ModuleManager&) = delete;
//
//     // 模块发现和注册
//     void DiscoverModules(const std::string& rootPath);
//
//     // 初始化所有模块
//     void InitializeModules();
//
//     // 清理所有模块
//     void CleanupModules();
//
//     // 向模块发送任务
//     void SendTaskToModule(const std::string& moduleName, const Task& task);
//
//     // 获取模块的依赖关系
//     const std::vector<std::string>& GetDependencies(const std::string& module) const;
//
// private:
//     enum class ModuleState {
//         Uninitialized,
//         Initializing,
//         Initialized,
//         Failed
//     };
//
//     // 注册模块（从元数据）
//     void RegisterModuleFromMetadata(const std::filesystem::path& metadataPath);
//
//     // 初始化单个模块
//     void InitializeModule(const std::string& moduleName);
//
//     // 拓扑排序，确定模块初始化顺序
//     bool TopologicalSort(std::vector<std::string>& sortedModules);
//
//     // 模块容器
//     std::unordered_map<std::string, std::unique_ptr<ModuleInterface>> modules;
//
//     // 模块状态
//     std::unordered_map<std::string, ModuleState> moduleStates;
//
//     // 模块依赖图
//     std::unordered_map<std::string, std::vector<std::string>> dependencyGraph;
//
//     // 已加载的模块句柄（用于动态库的卸载）
//     std::unordered_map<std::string, void*> loadedModules;
//
//     // 线程安全
//     mutable std::mutex moduleMutex;
// };
//
// #endif // MODULE_MANAGER_H

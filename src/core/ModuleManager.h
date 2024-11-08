#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_set>
#include "ModuleInterface.h"

namespace GE {

class ModuleManager {
public:
    ModuleManager() = default;
    ~ModuleManager();

    // 注册模块
    void RegisterModule(const std::string& name, std::unique_ptr<GE::ModuleInterface> module);

    // 卸载模块
    void UnregisterModule(const std::string& name);

    // 初始化所有模块
    void InitializeModules();

    // 清理所有模块
    void CleanupModules();


    void OnEvent(const std::string& event);


    void AddDependency(const std::string& module, const std::string& dependency);
    void RemoveDependency(const std::string& module, const std::string& dependency);


    void LoadModuleFromFile(const std::string& filePath, const std::string& moduleName);
    void UnloadModule(const std::string& name);

private:

    std::map<std::string, std::unique_ptr<GE::ModuleInterface>> modules;


    std::map<std::string, std::vector<std::string>> dependencyGraph;


    std::map<std::string, void*> loadedModules;


    std::mutex moduleMutex;


    bool TopologicalSort(std::vector<std::string>& sortedModules);
};

} // namespace MyEngine

#endif // MODULE_MANAGER_H

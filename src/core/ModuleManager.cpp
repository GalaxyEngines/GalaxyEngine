#include "ModuleManager.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include "ModuleInterface.h"

#ifdef _WIN32
    #include <windows.h>  // Windows下的动态库加载
#else
    #include <dlfcn.h>    // Linux下的动态库加载
#endif

namespace MyEngine {

// 析构函数，清理所有模块
ModuleManager::~ModuleManager() {
    CleanupModules();
}

// 注册模块
void ModuleManager::RegisterModule(const std::string& name, std::unique_ptr<ModuleInterface> module) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    if (modules.find(name) != modules.end()) {
        throw std::runtime_error("模块已注册: " + name);
    }
    modules[name] = std::move(module);
}

// 注销模块
void ModuleManager::UnregisterModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    auto it = modules.find(name);
    if (it != modules.end()) {
        it->second->shutdown();  // 确保模块的关闭
        modules.erase(it);
    }
}

// 初始化所有模块
void ModuleManager::InitializeModules() {
    std::lock_guard<std::mutex> lock(moduleMutex);
    std::vector<std::string> sortedModules;
    if (!TopologicalSort(sortedModules)) {
        throw std::runtime_error("检测到循环依赖");
    }

    for (const auto& moduleName : sortedModules) {
        if (modules.find(moduleName) != modules.end()) {
            modules[moduleName]->initialize();
        }
    }
}

// 清理所有模块
void ModuleManager::CleanupModules() {
    std::lock_guard<std::mutex> lock(moduleMutex);
    std::vector<std::string> sortedModules;
    if (!TopologicalSort(sortedModules)) {
        std::cerr << "检测到循环依赖，无法安全清理模块" << std::endl;
    }

    for (auto it = sortedModules.rbegin(); it != sortedModules.rend(); ++it) {
        const auto& moduleName = *it;
        if (modules.find(moduleName) != modules.end()) {
            modules[moduleName]->shutdown();
        }
    }
    modules.clear();
}

// 事件处理
void ModuleManager::OnEvent(const std::string& event) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    for (auto& pair : modules) {
        pair.second->onEvent(event);
    }
}

// 添加依赖关系
void ModuleManager::AddDependency(const std::string& module, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    dependencyGraph[module].push_back(dependency);
}

// 移除依赖关系
void ModuleManager::RemoveDependency(const std::string& module, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    auto& deps = dependencyGraph[module];
    deps.erase(std::remove(deps.begin(), deps.end(), dependency), deps.end());
}

// 从文件加载模块
void ModuleManager::LoadModuleFromFile(const std::string& filePath, const std::string& moduleName) {
#ifdef _WIN32
    HMODULE handle = LoadLibrary(filePath.c_str());
#else
    void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
#endif

    if (!handle) {
        throw std::runtime_error("加载模块失败: " + filePath);
    }

    using CreateModuleFunc = ModuleInterface* (*)();
#ifdef _WIN32
    CreateModuleFunc createModule = reinterpret_cast<CreateModuleFunc>(GetProcAddress(handle, "CreateModule"));
#else
    CreateModuleFunc createModule = reinterpret_cast<CreateModuleFunc>(dlsym(handle, "CreateModule"));
#endif

    if (!createModule) {
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        throw std::runtime_error("未找到 CreateModule 函数: " + filePath);
    }

    std::unique_ptr<ModuleInterface> modulePtr(createModule());
    if (!modulePtr) {
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        throw std::runtime_error("CreateModule 函数返回空指针: " + filePath);
    }

    RegisterModule(moduleName, std::move(modulePtr));
    loadedModules[moduleName] = handle;
}

// 卸载模块
void ModuleManager::UnloadModule(const std::string& name) {
    UnregisterModule(name);
    auto it = loadedModules.find(name);
    if (it != loadedModules.end()) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(it->second));
#else
        dlclose(it->second);
#endif
        loadedModules.erase(it);
    }
}

// 拓扑排序检查循环依赖
bool ModuleManager::TopologicalSort(std::vector<std::string>& sortedModules) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> onStack;
    bool hasCycle = false;

    std::function<void(const std::string&)> dfs = [&](const std::string& moduleName) {
        if (hasCycle) return;
        visited.insert(moduleName);
        onStack.insert(moduleName);

        for (const auto& dep : dependencyGraph[moduleName]) {
            if (onStack.find(dep) != onStack.end()) {
                hasCycle = true;
                return;
            }
            if (visited.find(dep) == visited.end()) {
                dfs(dep);
            }
        }

        onStack.erase(moduleName);
        sortedModules.push_back(moduleName);
    };

    for (const auto& pair : modules) {
        if (visited.find(pair.first) == visited.end()) {
            dfs(pair.first);
            if (hasCycle) {
                return false;
            }
        }
    }
    return true;
}

} // namespace MyEngine

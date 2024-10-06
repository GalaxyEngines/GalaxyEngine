// 优化后的 ModuleManager.cpp

#include "ModuleManager.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>

#include <stack>

ModuleManager::~ModuleManager() {
    CleanupModules();
}

void ModuleManager::RegisterModule(const std::string& name, std::unique_ptr<ModuleInterface> module) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    if (modules.find(name) != modules.end()) {
        throw std::runtime_error("模块已注册: " + name);
    }
    modules[name] = std::move(module);
    moduleStates[name] = ModuleState::Uninitialized;
}

void ModuleManager::UnregisterModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    auto it = modules.find(name);
    if (it != modules.end()) {
        try {
            it->second->Cleanup();
        } catch (const std::exception& e) {
            std::cerr << "清理模块失败: " << name << ": " << e.what() << std::endl;
        }
        modules.erase(it);
        moduleStates.erase(name);
    } else {
        std::cerr << "未找到模块: " << name << std::endl;
    }
}

void ModuleManager::InitializeModules() {
    std::lock_guard<std::mutex> lock(moduleMutex);
    // 拓扑排序以确定初始化顺序
    std::vector<std::string> sortedModules;
    if (!TopologicalSort(sortedModules)) {
        throw std::runtime_error("检测到循环依赖");
    }

    for (const auto& moduleName : sortedModules) {
        InitializeModule(moduleName);
    }
}

void ModuleManager::InitializeModule(const std::string& moduleName) {
    // 检查模块状态以避免重复初始化
    auto stateIt = moduleStates.find(moduleName);
    if (stateIt == moduleStates.end()) {
        throw std::runtime_error("模块未注册: " + moduleName);
    }
    if (stateIt->second == ModuleState::Initialized) {
        return; // 已经初始化
    }
    if (stateIt->second == ModuleState::Initializing) {
        throw std::runtime_error("检测到循环依赖: " + moduleName);
    }

    stateIt->second = ModuleState::Initializing;

    // 初始化依赖模块
    for (const auto& dependency : dependencyGraph[moduleName]) {
        InitializeModule(dependency);
    }

    try {
        modules[moduleName]->Initialize();
        stateIt->second = ModuleState::Initialized;
    } catch (const std::exception& e) {
        std::cerr << "初始化模块失败: " << moduleName << ": " << e.what() << std::endl;
        stateIt->second = ModuleState::Failed;
        throw; // 重新抛出异常
    }
}

void ModuleManager::CleanupModules() {
    std::lock_guard<std::mutex> lock(moduleMutex);
    // 按逆拓扑排序进行清理
    std::vector<std::string> sortedModules;
    if (!TopologicalSort(sortedModules)) {
        std::cerr << "检测到循环依赖，无法安全清理模块" << std::endl;
    }

    for (auto it = sortedModules.rbegin(); it != sortedModules.rend(); ++it) {
        const auto& moduleName = *it;
        auto stateIt = moduleStates.find(moduleName);
        if (stateIt != moduleStates.end() && stateIt->second == ModuleState::Initialized) {
            try {
                modules[moduleName]->Cleanup();
                stateIt->second = ModuleState::Uninitialized;
            } catch (const std::exception& e) {
                std::cerr << "清理模块失败: " << moduleName << ": " << e.what() << std::endl;
            }
        }
    }

    modules.clear();
    moduleStates.clear();

    // 卸载动态加载的模块
    for (auto& pair : loadedModules) {
        UnloadModule(pair.first);
    }
    loadedModules.clear();
}

void ModuleManager::OnEvent(const std::string& event) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    for (auto& pair : modules) {
        if (moduleStates[pair.first] == ModuleState::Initialized) {
            pair.second->OnEvent(event);
        }
    }
}

void ModuleManager::AddDependency(const std::string& module, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    dependencyGraph[module].push_back(dependency);
}

void ModuleManager::RemoveDependency(const std::string& module, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    auto& deps = dependencyGraph[module];
    deps.erase(std::remove(deps.begin(), deps.end(), dependency), deps.end());
}

const std::vector<std::string>& ModuleManager::GetDependencies(const std::string& module) const {
    std::lock_guard<std::mutex> lock(moduleMutex);
    auto it = dependencyGraph.find(module);
    if (it != dependencyGraph.end()) {
        return it->second;
    } else {
        static const std::vector<std::string> empty;
        return empty;
    }
}

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

void ModuleManager::UnloadModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(moduleMutex);
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

// 私有函数：拓扑排序
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

// ModuleManager.cpp

#include "ModuleManager.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

ModuleManager::~ModuleManager() {
    CleanupModules();
}

void ModuleManager::RegisterModule(const std::string& name, std::unique_ptr<ModuleInterface> module) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    if (modules.find(name) != modules.end()) {
        throw std::runtime_error("模块已注册: " + name);
    }
    modules[name] = std::move(module);
}

void ModuleManager::UnregisterModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    auto it = modules.find(name);
    if (it != modules.end()) {
        it->second->Cleanup();
        modules.erase(it);
    } else {
        std::cerr << "未找到模块: " << name << std::endl;
    }
}

void ModuleManager::InitializeModules() {
    std::lock_guard<std::mutex> lock(moduleMutex);
    for (const auto& pair : modules) {
        InitializeModule(pair.first);
    }
}

void ModuleManager::InitializeModule(const std::string& module) {
    // 处理依赖关系
    for (const auto& dependency : dependencyGraph[module]) {
        if (modules.find(dependency) != modules.end()) {
            InitializeModule(dependency);
        }
    }

    try {
        modules[module]->Initialize();
    } catch (const std::exception& e) {
        std::cerr << "初始化模块失败: " << module << ": " << e.what() << std::endl;
    }
}

void ModuleManager::CleanupModules() {
    std::lock_guard<std::mutex> lock(moduleMutex);
    for (auto& pair : modules) {
        pair.second->Cleanup();
    }
    modules.clear();
    // 卸载动态加载的模块
    for (auto& pair : loadedModules) {
        UnloadModule(pair.first);
    }
}

void ModuleManager::OnEvent(const std::string& event) {
    std::lock_guard<std::mutex> lock(moduleMutex);
    for (auto& pair : modules) {
        pair.second->OnEvent(event);
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
    return dependencyGraph.at(module);
}

void ModuleManager::LoadModuleFromFile(const std::string& filePath) {
#ifdef _WIN32
    HMODULE handle = LoadLibrary(filePath.c_str());
#else
    void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
#endif

    if (!handle) {
        throw std::runtime_error("加载模块失败: " + filePath);
    }

    using CreateModuleFunc = ModuleInterface* (*)();
    CreateModuleFunc createModule = (CreateModuleFunc)dlsym(handle, "CreateModule");
    if (!createModule) {
        throw std::runtime_error("未找到 CreateModule 函数: " + filePath);
    }

    ModuleInterface* module = createModule();
    RegisterModule("DynamicModule", std::unique_ptr<ModuleInterface>(module));
    loadedModules["DynamicModule"] = handle;
}

void ModuleManager::UnloadModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(moduleMutex);
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

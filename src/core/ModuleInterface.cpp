#include "ModuleInterface.h"
#include <iostream>

namespace GE {

ModuleInterface::ModuleInterface() : initialized(false), active(false) {
    std::cout << "ModuleInterface 模块被创建" << std::endl;
}

ModuleInterface::~ModuleInterface() {
    std::cout << "ModuleInterface 模块被销毁" << std::endl;
}

void ModuleInterface::initialize() {
    if (initialized) {
        std::cerr << "Module 已经初始化，无法重复初始化。" << std::endl;
        return;
    }
    std::cout << "ModuleInterface 模块初始化" << std::endl;
    initialized = true;
    active = true;
}

void ModuleInterface::shutdown() {
    if (!initialized || !active) {
        std::cerr << "Module 尚未初始化或已关闭，无法重复关闭。" << std::endl;
        return;
    }
    std::cout << "ModuleInterface 模块关闭" << std::endl;
    active = false;
    initialized = false;
}

void ModuleInterface::update() {
    if (!initialized || !active) {
        std::cerr << "Module 尚未初始化或已关闭，无法更新。" << std::endl;
        return;
    }
    std::cout << "ModuleInterface 模块更新" << std::endl;
}

void ModuleInterface::onEvent(const std::string& event) {
    if (!active) {
        std::cerr << "Module 非活动状态，无法处理事件。" << std::endl;
        return;
    }
    std::cout << "ModuleInterface 处理事件: " << event << std::endl;
}

void ModuleInterface::processTask(const Task& task) {
    if (!active) {
        std::cerr << "Module 非活动状态，无法处理任务。" << std::endl;
        return;
    }
    std::cout << "ModuleInterface 处理任务: " << task.GetData() << std::endl;
}

} // namespace GE

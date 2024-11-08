#include "ModuleInterface.h"
#include <iostream>
#include <stdexcept>  // 引入标准异常库

namespace GE {

// 构造函数，初始化模块
ModuleInterface::ModuleInterface() : initialized(false), active(false) {
    std::cout << "ModuleInterface 模块被创建" << std::endl;
}

// 析构函数，不能调用虚函数
ModuleInterface::~ModuleInterface() {
    std::cout << "ModuleInterface 模块被销毁" << std::endl;
    // 不在析构函数中调用虚函数
}

// 模块的初始化方法，子类可以重写进行具体的初始化操作
void ModuleInterface::initialize() {
    if (initialized) {
        std::cerr << "Module 已经初始化，无法重复初始化。" << std::endl;
        return;
    }

    try {
        // 执行初始化逻辑，子类实现时应保证无异常
        std::cout << "ModuleInterface 模块初始化" << std::endl;
        initialized = true;
        active = true;
    } catch (const std::exception& e) {
        std::cerr << "模块初始化失败: " << e.what() << std::endl;
        throw;  // 向上传递异常，确保外部知道初始化失败
    }
}

// 模块的更新方法，确保模块处于活跃状态时才能更新
void ModuleInterface::update() {
    if (!initialized || !active) {
        std::cerr << "Module 尚未初始化或已关闭，无法更新。" << std::endl;
        return;
    }

    try {
        // 执行更新逻辑，子类应实现具体的更新行为
        std::cout << "ModuleInterface 模块更新" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "模块更新过程中发生错误: " << e.what() << std::endl;
        // 可选：在更新失败后可能停止模块
        active = false;
    }
}

// 模块的关闭和清理方法，确保只关闭一次
void ModuleInterface::shutdown() {
    if (!initialized || !active) {
        std::cerr << "Module 尚未初始化或已关闭，无法重复关闭。" << std::endl;
        return;
    }

    try {
        // 执行关闭和资源清理逻辑
        std::cout << "ModuleInterface 模块关闭" << std::endl;
        active = false;
        initialized = false;
    } catch (const std::exception& e) {
        std::cerr << "模块关闭过程中发生错误: " << e.what() << std::endl;
        throw;  // 可能的资源清理异常需要引起注意
    }
}

// 处理事件的虚拟方法，子类可以重写此方法以处理特定事件
void ModuleInterface::onEvent(const std::string& event) {
    if (!active) {
        std::cerr << "Module 非活动状态，无法处理事件。" << std::endl;
        return;
    }

    std::cout << "ModuleInterface 处理事件: " << event << std::endl;
    // 子类可根据事件的类型进行具体的处理逻辑
}

// 模块的任务处理方法，接受一个 Task 对象并处理任务
void ModuleInterface::processTask(const Task& task) {
    if (!active) {
        std::cerr << "Module 非活动状态，无法处理任务。" << std::endl;
        return;
    }

    try {
        std::cout << "ModuleInterface 处理任务: " << task.GetData() << std::endl;
        // 子类可以重写此方法，根据任务数据执行具体任务
    } catch (const std::exception& e) {
        std::cerr << "模块任务处理过程中发生错误: " << e.what() << std::endl;
        // 错误处理逻辑，可以记录日志或者标记任务失败
    }
}

}  // namespace MyEngine

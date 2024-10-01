#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <string>
#include <memory>
#include <functional>

// 任务类定义
class Task {
public:
    // 任务类型（根据实际需求扩展）
    enum class TaskType {
        LoadResource,
        RenderFrame,
        ProcessInput,
        MemoryOperation,
        // 其他任务类型...
    };

    // 构造函数
    Task(TaskType type, const std::string& data)
        : type(type), data(data) {}

    // 获取任务类型
    TaskType GetType() const { return type; }

    // 获取任务数据
    const std::string& GetData() const { return data; }

private:
    TaskType type;
    std::string data; // 用于存储与任务相关的任意数据（序列化后的字符串）
};

// 模块接口定义
class ModuleInterface {
public:
    virtual ~ModuleInterface() = default;

    // 模块初始化
    virtual void Initialize() = 0;

    // 模块清理
    virtual void Cleanup() = 0;

    // 处理事件
    virtual void OnEvent(const std::string& event) = 0;

    // 处理任务
    virtual void ProcessTask(const Task& task) = 0;
};

// 定义 ModuleInterface 的创建接口
extern "C" {
    // 模块工厂函数，用于动态创建模块
    ModuleInterface* CreateModule();
}

#endif // MODULE_INTERFACE_H


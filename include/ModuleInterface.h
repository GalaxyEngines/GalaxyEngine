#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <string>

// 任务类定义
class Task {
public:
    // 任务类型（可以根据需要扩展）
    enum class TaskType {
        LoadResource,
        RenderFrame,
        ProcessInput,
        // 其他任务类型...
    };

    Task(TaskType type, const std::string& data)
        : type(type), data(data) {}

    TaskType GetType() const { return type; }
    const std::string& GetData() const { return data; }

private:
    TaskType type;
    std::string data;
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

#endif // MODULE_INTERFACE_H

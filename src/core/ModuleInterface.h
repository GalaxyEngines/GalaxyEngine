#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <string>

namespace MyEngine {

    // 模块的基类，定义了所有模块的通用接口
    class ModuleInterface {
    public:
        // 构造函数和析构函数
        ModuleInterface();
        virtual ~ModuleInterface();

        // 模块生命周期管理方法，供派生类重写
        virtual void initialize() = 0;  // 初始化模块
        virtual void shutdown() = 0;    // 关闭并清理模块

        // 虚拟的事件处理方法，供派生类重写
        virtual void onEvent(const std::string& event) = 0;

        // 任务处理方法，供派生类重写，处理传递的任务数据
        virtual void processTask(const class Task& task) = 0;

        // 如果 update 方法是纯虚函数
        virtual void update() = 0;

    protected:
        bool initialized;  // 标志模块是否已初始化
        bool active;       // 标志模块是否处于活跃状态
    };

    // 任务类
    class Task {
    public:
        enum class TaskType { LoadResource, Compute, renderFrame };

        Task(const std::string& data, TaskType type) : data_(data), type_(type) {}

        const std::string& GetData() const { return data_; }
        TaskType GetType() const { return type_; }

    private:
        std::string data_;
        TaskType type_;
    };

}

#endif // MODULE_INTERFACE_H

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <string>
#include "Task.h"  // 假设 Task 是一个你已经定义的任务类

// 模块的基类，定义了所有模块的通用接口
class ModuleInterface {
public:
    // 构造函数和析构函数
    ModuleInterface();
    virtual ~ModuleInterface();

    // 虚拟的模块生命周期管理方法，供派生类重写
    virtual void initialize();  // 初始化模块
    virtual void update();      // 每帧更新模块状态
    virtual void shutdown();    // 关闭并清理模块

    // 虚拟的事件处理方法，供派生类重写
    virtual void onEvent(const std::string& event);

    // 任务处理方法，接受一个 Task 对象并处理任务，供派生类重写
    virtual void processTask(const Task& task);

protected:
    bool initialized;  // 标志模块是否已初始化
    bool active;       // 标志模块是否处于活跃状态
};

#endif // MODULE_INTERFACE_H

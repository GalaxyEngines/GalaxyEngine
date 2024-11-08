#ifndef FIBERMANAGER_H
#define FIBERMANAGER_H

#include <functional>
#include <iostream>

namespace GE {

class FiberManager {
public:
    virtual ~FiberManager() = default;

    // 运行 Fiber 任务
    virtual void RunFiber(std::function<void()> fiberFunc) {
        std::cout << "Running fiber task (default implementation)." << std::endl;
        fiberFunc();
    }

    // 暂停所有 Fibers
    virtual void PauseFibers() {
        std::cout << "Pausing all fibers (default implementation)." << std::endl;
        // 默认实现：空操作，派生类可以选择重写
    }

    // 恢复所有 Fibers
    virtual void ResumeFibers() {
        std::cout << "Resuming all fibers (default implementation)." << std::endl;
        // 默认实现：空操作，派生类可以选择重写
    }

    // 关闭 Fiber 管理器
    void shutdown() {
        std::cout << "Shutting down FiberManager." << std::endl;
    }
};

} // namespace GE

#endif // FIBERMANAGER_H

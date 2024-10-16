#ifndef FIBERMANAGER_H
#define FIBERMANAGER_H

#include <functional>

class FiberManager {
public:
    virtual ~FiberManager() = default;

    // 运行 Fiber 任务
    virtual void RunFiber(std::function<void()> fiberFunc) = 0;

    // 暂停所有 Fibers
    virtual void PauseFibers() = 0;

    // 恢复所有 Fibers
    virtual void ResumeFibers() = 0;
};

#endif // FIBERMANAGER_H

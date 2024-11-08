#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <mutex>
namespace MyEngine {
    class MemoryPool;
class Memory {
public:
    virtual ~Memory() = default;
    // 分配内存
    virtual void* AllocateMemory(std::size_t size) = 0;
    // 释放内存
    virtual void FreeMemory(void* ptr, std::size_t size) = 0;
};

// 内存管理器类，继承 Memory 基类
class MemoryManager : public Memory {
public:
    MemoryManager();
    ~MemoryManager();

    // 实现分配内存
    void* AllocateMemory(std::size_t size) override;

    // 实现释放内存
    void FreeMemory(void* ptr, std::size_t size) override;

private:
    // 内存池的集合，根据大小进行管理
    std::unordered_map<std::size_t, std::unique_ptr<MemoryPool>> memoryPools;
    std::mutex memoryMutex;

    // 获取合适的内存池
    MemoryPool* GetPool(std::size_t size);

    // 扩展内存池以满足需求
    void ExpandPool(std::size_t size, std::size_t count);
};

}

#endif //

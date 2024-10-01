#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <cstddef>

class MemoryManager {
public:
    virtual ~MemoryManager() = default;

    // 分配内存
    virtual void* AllocateMemory(std::size_t size) = 0;

    // 释放内存
    virtual void FreeMemory(void* ptr, std::size_t size) = 0;
};

#endif // MEMORYMANAGER_H

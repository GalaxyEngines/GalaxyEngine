#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <mutex>
namespace GE {
    class MemoryPool;
class Memory {
public:
    virtual ~Memory() = default;

    virtual void* AllocateMemory(std::size_t size) = 0;

    virtual void FreeMemory(void* ptr, std::size_t size) = 0;
};


class MemoryManager : public Memory {
public:
    MemoryManager();
    ~MemoryManager();


    void* AllocateMemory(std::size_t size) override;


    void FreeMemory(void* ptr, std::size_t size) override;

private:

    std::unordered_map<std::size_t, std::unique_ptr<MemoryPool>> memoryPools;
    std::mutex memoryMutex;


    MemoryPool* GetPool(std::size_t size);


    void ExpandPool(std::size_t size, std::size_t count);
};

}

#endif //

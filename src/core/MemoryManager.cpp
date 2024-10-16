#include "ModuleInterface.h"
#include <mutex>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <tuple>

class MemoryPool {
public:
    MemoryPool(size_t blockSize, size_t initialBlocks = 1024)
        : blockSize(blockSize) {
        ExpandPool(initialBlocks);
    }

    ~MemoryPool() {
        for (void* block : blocks) {
            ::operator delete(block);
        }
    }

    void* Allocate() {
        std::lock_guard<std::mutex> lock(mutex);
        if (freeBlocks.empty()) {
            ExpandPool(blocks.size());
        }
        void* ptr = freeBlocks.back();
        freeBlocks.pop_back();
        return ptr;
    }

    void Deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        freeBlocks.push_back(ptr);
    }

private:
    void ExpandPool(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            void* block = ::operator new(blockSize);
            freeBlocks.push_back(block);
            blocks.push_back(block);
        }
    }

    size_t blockSize;
    std::vector<void*> freeBlocks;
    std::vector<void*> blocks;
    std::mutex mutex;
};

// MemoryManagerModule 继承自已定义的 ModuleInterface
class MemoryManagerModule : public MyEngine::ModuleInterface {  // 修复 C2504 错误，确保继承正确
public:
    void Initialize() override {  // 修复 C3668 错误
        pools.emplace(64, std::make_unique<MemoryPool>(64));
        pools.emplace(256, std::make_unique<MemoryPool>(256));
        pools.emplace(1024, std::make_unique<MemoryPool>(1024));
        std::cout << "Memory Initialized with memory pools" << std::endl;
    }

    void Cleanup() override {  // 修复 C3668 错误
        pools.clear();
        std::cout << "Memory Cleaned up." << std::endl;
    }

    void OnEvent(const std::string& event) override {  // 修复 C3668 错误
        std::cout << "Received memory event: " << event << std::endl;
    }

    void ProcessTask(const MyEngine::Task& task) override {  // 修复 C3668 和 C2065 错误
        std::string operation;
        size_t size;
        void* pointer;
        std::tie(operation, size, pointer) = ParseMemoryTaskData(task.GetData());

        if (operation == "allocate") {
            void* allocatedPtr = AllocateMemory(size);
            std::cout << "Allocated memory of size: " << size << std::endl;
        }
        else if (operation == "free") {
            FreeMemory(pointer, size);
            std::cout << "Freed memory of size: " << size << std::endl;
        }
    }

    void* AllocateMemory(size_t size) {
        auto pool = GetPool(size);
        if (pool) {
            return pool->Allocate();
        }
        else {
            return ::operator new(size);
        }
    }

    void FreeMemory(void* ptr, size_t size) {
        auto pool = GetPool(size);
        if (pool) {
            pool->Deallocate(ptr);
        }
        else {
            ::operator delete(ptr);
        }
    }

private:
    std::unordered_map<size_t, std::unique_ptr<MemoryPool>> pools;

    MemoryPool* GetPool(size_t size) {
        for (auto& [poolSize, pool] : pools) {
            if (size <= poolSize) {
                return pool.get();
            }
        }
        return nullptr;
    }

    std::tuple<std::string, size_t, void*> ParseMemoryTaskData(const std::string& data) {
        std::string operation = "allocate";
        size_t size = 256;
        void* pointer = nullptr;

        return { operation, size, pointer };
    }
};

// 动态创建模块实例
extern "C" MyEngine::ModuleInterface* CreateModule() {
    return new MemoryManagerModule();
}

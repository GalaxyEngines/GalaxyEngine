#include "ModuleInterface.h"
#include <mutex>
#include <unordered_map>
#include <iostream>
#include <memory>

using namespace MyEngine;

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
            ExpandPool(blocks.size());  // 双倍扩展内存池大小
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

class MemoryManagerModule : public ModuleInterface {
public:
    void Initialize() override {
        pools.emplace(64, std::make_unique<MemoryPool>(64));
        pools.emplace(256, std::make_unique<MemoryPool>(256));
        pools.emplace(1024, std::make_unique<MemoryPool>(1024));
        std::cout << "MemoryManager Initialized with memory pools" << std::endl;
    }

    void Cleanup() override {
        pools.clear();
        std::cout << "MemoryManager Cleaned up." << std::endl;
    }

    void OnEvent(const std::string& event) override {
        std::cout << "Received memory event: " << event << std::endl;
    }

    void ProcessTask(const Task& task) override {
        auto [operation, size, pointer] = ParseMemoryTaskData(task.GetData());
        if (operation == "allocate") {
            void* allocatedPtr = AllocateMemory(size);
            // 返回给请求者处理或存储指针
            std::cout << "Allocated memory of size: " << size << std::endl;
        } else if (operation == "free") {
            FreeMemory(pointer, size);
            std::cout << "Freed memory of size: " << size << std::endl;
        }
    }

    // 提供给UI的API接口
    void* AllocateMemory(size_t size) {
        auto pool = GetPool(size);
        if (pool) {
            return pool->Allocate();
        } else {
            std::cout << "Allocating memory without pool for size: " << size << std::endl;
            return ::operator new(size);
        }
    }

    void FreeMemory(void* ptr, size_t size) {
        auto pool = GetPool(size);
        if (pool) {
            pool->Deallocate(ptr);
        } else {
            std::cout << "Freeing memory without pool for size: " << size << std::endl;
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
        // 更好的数据格式使用JSON或类似的序列化方式,这块以后写吧
        // 模拟解析逻辑
        std::string operation = "allocate"; // 假设操作来自数据
        size_t size = 256; // 假设大小来自数据
        void* pointer = nullptr;
        return {operation, size, pointer};
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new MemoryManagerModule();
}

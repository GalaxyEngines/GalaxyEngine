#include "ModuleInterface.h"
#include <mutex>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>  // 修复 'vector' 未解析符号的错误

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
class MemoryManagerModule : public MyEngine::ModuleInterface {
public:
    void initialize() override {  // 实现 initialize() 以替换纯虚函数
        pools.emplace(64, std::make_unique<MemoryPool>(64));
        pools.emplace(256, std::make_unique<MemoryPool>(256));
        pools.emplace(1024, std::make_unique<MemoryPool>(1024));
        std::cout << "Memory Initialized with memory pools" << std::endl;
    }

    void shutdown() override {  // 实现 shutdown() 以替换纯虚函数
        pools.clear();
        std::cout << "Memory Cleaned up." << std::endl;
    }

    void onEvent(const std::string& event) override {  // 实现 onEvent() 以替换纯虚函数
        std::cout << "Received memory event: " << event << std::endl;
    }

    void processTask(const MyEngine::Task& task) override {  // 实现 processTask() 以替换纯虚函数
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

    // 添加 update 函数以实现纯虚函数
    void update() override {
        std::cout << "MemoryManagerModule update called." << std::endl;
        // 更新模块的逻辑，可以根据需要实现
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
        std::string operation = "allocate";  // 模拟解析，默认操作为分配
        size_t size = 256;
        void* pointer = nullptr;

        return { operation, size, pointer };
    }
};

// 动态创建模块实例
extern "C" MyEngine::ModuleInterface* CreateModule() {
    return new MemoryManagerModule();
}

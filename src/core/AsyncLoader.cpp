#include "ModuleInterface.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <future>
#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <atomic>

class AsyncLoaderModule : public ModuleInterface {
public:
    AsyncLoaderModule() : stopLoading(false) {}

    void Initialize() override {
        unsigned int threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 4;
        for (unsigned int i = 0; i < threadCount; ++i) {
            loaderThreads.emplace_back(&AsyncLoaderModule::LoaderThreadFunc, this);
        }
    }

    void Cleanup() override {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopLoading = true;
        }
        cv.notify_all();
        for (auto& thread : loaderThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void OnEvent(const std::string& event) override {}

    void ProcessTask(const Task& task) override {
        if (task.GetType() == Task::TaskType::LoadResource) {
            auto [resourcePath, callback] = ParseLoadTaskData(task.GetData());
            EnqueueLoadTask(resourcePath, callback);
        }
    }

    void LoadResourceAsync(const std::string& resourcePath, std::function<void(std::shared_ptr<std::vector<char>>)> callback) {
        EnqueueLoadTask(resourcePath, callback);
    }

private:
    struct LoadTask {
        std::string resourcePath;
        std::function<void(std::shared_ptr<std::vector<char>>)> callback;
    };

    std::queue<LoadTask> loadQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> stopLoading;

    std::vector<std::thread> loaderThreads;

    std::unordered_map<std::string, std::shared_ptr<std::vector<char>>> resourceCache;
    std::mutex cacheMutex;

    void EnqueueLoadTask(const std::string& resourcePath, std::function<void(std::shared_ptr<std::vector<char>>)> callback) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            loadQueue.push({resourcePath, callback});
        }
        cv.notify_one();
    }

    void LoaderThreadFunc() {
        while (true) {
            LoadTask task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this]() { return stopLoading.load() || !loadQueue.empty(); });

                if (stopLoading.load() && loadQueue.empty()) {
                    break;
                }

                task = loadQueue.front();
                loadQueue.pop();
            }

            std::shared_ptr<std::vector<char>> resourceData;
            {
                std::lock_guard<std::mutex> lock(cacheMutex);
                auto it = resourceCache.find(task.resourcePath);
                if (it != resourceCache.end()) {
                    resourceData = it->second;
                }
            }

            if (!resourceData) {
                resourceData = LoadResource(task.resourcePath);
                if (resourceData) {
                    std::lock_guard<std::mutex> lock(cacheMutex);
                    resourceCache[task.resourcePath] = resourceData;
                }
            }

            if (task.callback) {
                task.callback(resourceData);
            }
        }
    }

    std::shared_ptr<std::vector<char>> LoadResource(const std::string& resourcePath) {
        std::ifstream file(resourcePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "无法打开资源文件: " << resourcePath << std::endl;
            return nullptr;
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (size == 0) {
            std::cerr << "资源文件为空: " << resourcePath << std::endl;
            return nullptr;
        }

        auto buffer = std::make_shared<std::vector<char>>(size);
        if (!file.read(buffer->data(), size)) {
            std::cerr << "读取资源文件失败: " << resourcePath << std::endl;
            return nullptr;
        }

        return buffer;
    }

    std::pair<std::string, std::function<void(std::shared_ptr<std::vector<char>>)>> ParseLoadTaskData(const std::string& data) {
        std::string resourcePath = data;
        std::function<void(std::shared_ptr<std::vector<char>>)> callback = nullptr;
        return {resourcePath, callback};
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new AsyncLoaderModule();
}

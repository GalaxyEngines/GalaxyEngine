#include "ModuleInterface.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <iostream>
#include <nlohmann/json.hpp> // 使用数据格式（JSON）

using json = nlohmann::json;
using namespace MyEngine;

class TaskSchedulerModule : public ModuleInterface {
public:
    TaskSchedulerModule() : stop(false) {}

    void Initialize() override {
        unsigned int threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 4;
        for (unsigned int i = 0; i < threadCount; ++i) {
            workers.emplace_back(&TaskSchedulerModule::WorkerThreadFunc, this);
        }
        std::cout << "TaskScheduler Initialized with " << threadCount << " threads." << std::endl;
    }

    void Cleanup() override {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        cv.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        std::cout << "TaskScheduler Fucked up." << std::endl;
    }

    void OnEvent(const std::string& event) override {
        std::cout << "Received event: " << event << std::endl;
        if (event == "shutdown") {
            StopScheduler();
        }
    }

    void ProcessTask(const Task& task) override {
        auto parsedTask = ParseTaskData(task.GetData());
        EnqueueTask([parsedTask]() {
            // 执行解析后的任务逻辑
            std::cout << "Executing parsed task: " << parsedTask << std::endl;
        });
    }

    // 提供给UI的API接口
    template<typename Func, typename... Args>
    auto ScheduleTask(Func&& func, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type> {
        using return_type = typename std::result_of<Func(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace([task]() { (*task)(); });
        }
        cv.notify_one();
        return res;
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> stop;

    void EnqueueTask(std::function<void()> func) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace(func);
        }
        cv.notify_one();
    }

    void WorkerThreadFunc() {
        while (true) {
            std::function<void()> taskFunc;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this]() { return stop.load() || !tasks.empty(); });
                if (stop.load() && tasks.empty()) {
                    break;
                }
                taskFunc = std::move(tasks.front());
                tasks.pop();
            }
            taskFunc();
        }
    }

    void StopScheduler() {
        std::cout << "fucking TaskScheduler..." << std::endl;
        stop = true;
        cv.notify_all();
    }

    std::string ParseTaskData(const std::string& data) {
        json parsedData = json::parse(data);
        std::string taskType = parsedData["task_type"];
        return taskType; // 返回任务类型作为解析结果
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new TaskSchedulerModule();
}

#include "ModuleInterface.h"
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <atomic>
#include <iostream>
#include <boost/context/fiber.hpp>

using namespace MyEngine;

class FiberManagerModule : public ModuleInterface {
public:
    FiberManagerModule() : stop(false) {}

    void Initialize() override {
        schedulerThread = std::thread(&FiberManagerModule::SchedulerThreadFunc, this);
    }

    void Cleanup() override {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        cv.notify_all();
        if (schedulerThread.joinable()) {
            schedulerThread.join();
        }
    }

    void OnEvent(const std::string& event) override {
        // 处理事件（没做）
    }

    void ProcessTask(const Task& task) override {
        if (task.GetType() == Task::TaskType::RunFiber) {
            auto fiberFunc = GetFiberFunction(task.GetData());
            EnqueueFiberTask(fiberFunc);
        }
    }

    // 提供给UI的API接口
    void RunFiber(std::function<void()> fiberFunc) {
        EnqueueFiberTask(fiberFunc);
    }

private:
    using Fiber = boost::context::fiber;

    std::queue<std::function<void()>> fiberTasks;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> stop;

    std::thread schedulerThread;

    void EnqueueFiberTask(std::function<void()> func) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            fiberTasks.push(func);
        }
        cv.notify_one();
    }

    void SchedulerThreadFunc() {
        while (true) {
            std::function<void()> taskFunc;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this]() { return stop.load() || !fiberTasks.empty(); });
                if (stop.load() && fiberTasks.empty()) {
                    break;
                }
                taskFunc = fiberTasks.front();
                fiberTasks.pop();
            }

            Fiber fiber = Fiber([taskFunc](Fiber&& sink) mutable {
                taskFunc();
                return std::move(sink);
            });

            fiber = std::move(fiber).resume();
        }
    }

    std::function<void()> GetFiberFunction(const std::string& data) {
        // 根据数据获取Fiber函数
        return []() {
            // Fiber的实际功能，我还没做
        };
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new FiberManagerModule();
}

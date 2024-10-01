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
#include <nlohmann/json.hpp> // 用于解析更好的数据格式（JSON）

using json = nlohmann::json;
using namespace MyEngine;

class FiberManagerModule : public ModuleInterface {
public:
    FiberManagerModule() : stop(false) {}

    void Initialize() override {
        schedulerThread = std::thread(&FiberManagerModule::SchedulerThreadFunc, this);
        std::cout << "FiberManagerModule 上号了！." << std::endl;
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
        std::cout << "FiberManagerModule Fucked up." << std::endl;
    }

    void OnEvent(const std::string& event) override {
        std::cout << "FiberManager received event: " << event << std::endl;
        if (event == "pause") {
            PauseFibers();
        } else if (event == "resume") {
            ResumeFibers();
        }
    }

    void ProcessTask(const Task& task) override {
        auto fiberFunc = ParseFiberTaskData(task.GetData());
        EnqueueFiberTask(fiberFunc);
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

    void PauseFibers() {
        std::cout << "fucking all fibers..." << std::endl;
        // 暂停所有fiber的逻辑
    }

    void ResumeFibers() {
        std::cout << "fuhuo all fibers..." << std::endl;
        // 恢复所有fiber的逻辑
    }

    std::function<void()> ParseFiberTaskData(const std::string& data) {
        json parsedData = json::parse(data);
        std::string action = parsedData["action"];
        if (action == "complex_computation") {
            return []() {
                std::cout << "光纤复杂计算..." << std::endl;
                // 在这里实现fiber的实际功能，这地方需要计算的
                Fiber::yield(); // 切换上下文，可以调用yield
            };
        }
        return []() {
            std::cout << "Running default fiber task..." << std::endl;
        };
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new FiberManagerModule();
}

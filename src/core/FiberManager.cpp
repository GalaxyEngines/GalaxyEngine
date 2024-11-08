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
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace GE;

class FiberManagerModule : public ModuleInterface {
public:
    FiberManagerModule() : stop(false) {}

    void initialize() override {
        schedulerThread = std::thread(&FiberManagerModule::SchedulerThreadFunc, this);
        std::cout << "FiberManagerModule ." << std::endl;
    }

    void shutdown() override {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        cv.notify_all();
        if (schedulerThread.joinable()) {
            schedulerThread.join();
        }
        std::cout << "FiberManagerModule Cleaned up." << std::endl;
    }

    void onEvent(const std::string& event) override {
        std::cout << "FiberManager received event: " << event << std::endl;
        if (event == "pause") {
            PauseFibers();
        } else if (event == "resume") {
            ResumeFibers();
        }
    }

    void processTask(const std::string& taskData) override {
        auto fiberFunc = ParseFiberTaskData(taskData);
        EnqueueFiberTask(fiberFunc);
    }

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

            while (fiber) {
                fiber = std::move(fiber).resume();
            }
        }
    }

    void PauseFibers() {
        std::cout << "Pausing all fibers..." << std::endl;
    }

    void ResumeFibers() {
        std::cout << "Resuming all fibers..." << std::endl;
    }

    std::function<void()> ParseFiberTaskData(const std::string& data) {
        json parsedData = json::parse(data);
        std::string action = parsedData["action"];
        if (action == "complex_computation") {
            return []() {
                std::cout << "Fiber complex computation..." << std::endl;
            };
        }
        return []() {
            std::cout << "boom fuck fiber task..." << std::endl;
        };
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new FiberManagerModule();
}

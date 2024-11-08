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

namespace GE {

using json = nlohmann::json;

class TaskSchedulerModule : public ModuleInterface {
public:
    TaskSchedulerModule() : stop(false) {}

    void initialize() override {
        unsigned int threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 4; // 默认使用 4 个线程
        for (unsigned int i = 0; i < threadCount; ++i) {
            workers.emplace_back(&TaskSchedulerModule::WorkerThreadFunc, this);
        }
        std::cout << "任务调度器初始化 " << threadCount << " threads." << std::endl;
    }

    void shutdown() override {
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
        std::cout << "TaskScheduler 已关闭。" << std::endl;
    }

    void onEvent(const std::string& event) override {
        std::cout << "Received event: " << event << std::endl;
        if (event == "shutdown") {
            StopScheduler();
        }
    }

    void processTask(const Task& task) override {
        auto parsedTask = ParseTaskData(task.GetData());
        EnqueueTask([parsedTask]() {
            std::cout << "正在执行解析任务: " << parsedTask << std::endl;
        });
    }

    void update() override {
        std::cout << "TaskSchedulerModule updated." << std::endl;
    }

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
                if (!tasks.empty()) {
                    taskFunc = std::move(tasks.front());
                    tasks.pop();
                }
            }
            if (taskFunc) {
                taskFunc();
            }
        }
    }

    std::string ParseTaskData(const std::string& data) {
        try {
            json parsedData = json::parse(data);
            std::string taskType = parsedData["task_type"];
            return taskType;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse 错误: " << e.what() << std::endl;
            return "invalid";
        }
    }

    void StopScheduler() {
        std::cout << "Stopping TaskScheduler..." << std::endl;
        stop = true;
        cv.notify_all();
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new TaskSchedulerModule();
}

} // namespace GE

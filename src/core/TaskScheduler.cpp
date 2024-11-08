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
using namespace GE;

class TaskSchedulerModule : public ModuleInterface {
public:
    TaskSchedulerModule() : stop(false) {}

    void initialize() override {  // 修正命名
        unsigned int threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 4; // 默认使用 4 个线程
        for (unsigned int i = 0; i < threadCount; ++i) {
            workers.emplace_back(&TaskSchedulerModule::WorkerThreadFunc, this);
        }
        std::cout << "任务调度器初始化 " << threadCount << " threads." << std::endl;
    }

    void shutdown() override {  // 修正命名
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

    void onEvent(const std::string& event) override {  // 修正命名
        std::cout << "Received event: " << event << std::endl;
        if (event == "shutdown") {
            StopScheduler();
        }
    }

    void processTask(const Task& task) override {  // 修正命名和签名
        auto parsedTask = ParseTaskData(task.GetData());
        EnqueueTask([parsedTask]() {
            // 执行解析后的任务逻辑
            std::cout << "正在执行解析任务: " << parsedTask << std::endl;
        });
    }

    // 实现 update() 以符合纯虚函数要求
    void update() override {
        // 任务调度器的更新逻辑可以放在这里
        std::cout << "TaskSchedulerModule updated." << std::endl;
    }

    // 提供给UI的API接口，提交任务并返回 future 以获取结果
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

    // 将任务加入队列
    void EnqueueTask(std::function<void()> func) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace(func);
        }
        cv.notify_one(); // 唤醒一个等待的线程来处理任务
    }

    // 工作线程的主循环函数
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
                taskFunc(); // 执行任务
            }
        }
    }

    // 解析 JSON 格式的任务数据
    std::string ParseTaskData(const std::string& data) {
        try {
            json parsedData = json::parse(data);
            std::string taskType = parsedData["task_type"];
            return taskType; // 返回任务类型作为解析结果
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse 错误: " << e.what() << std::endl;
            return "invalid";
        }
    }

    // 停止任务调度器
    void StopScheduler() {
        std::cout << "Stopping TaskScheduler..." << std::endl;
        stop = true;
        cv.notify_all();
    }
};

extern "C" ModuleInterface* CreateModule() {
    return new TaskSchedulerModule();
}

#ifndef TASKSCHEDULERMODULE_H
#define TASKSCHEDULERMODULE_H

#include "ModuleInterface.h"
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>

namespace GE {

class TaskSchedulerModule : public ModuleInterface {
public:
    TaskSchedulerModule();
    ~TaskSchedulerModule() override;


    void initialize() override;


    void shutdown() override;


    void onEvent(const std::string& event) override;


    void processTask(const Task& task) override;


    template<typename Func, typename... Args>
    auto ScheduleTask(Func&& func, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type>;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> stop;


    void EnqueueTask(std::function<void()> func);


    void WorkerThreadFunc();


    void StopScheduler();


    std::string ParseTaskData(const std::string& data);
};


template<typename Func, typename... Args>
auto TaskSchedulerModule::ScheduleTask(Func&& func, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type> {
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

}
#endif // TASKSCHEDULERMODULE_H

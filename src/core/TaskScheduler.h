// TaskScheduler.h

#pragma once

#include <functional>
#include <future>
#include <queue>
#include <deque>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

class TaskScheduler {
public:
    TaskScheduler(size_t threadCount = std::thread::hardware_concurrency());
    ~TaskScheduler();

    template<typename Func, typename... Args>
    auto SubmitTask(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>;

private:
    struct ThreadData {
        std::thread worker;
        std::deque<std::function<void()>> taskQueue;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool stopFlag = false;
    };

    std::vector<std::unique_ptr<ThreadData>> threads;
    std::atomic<bool> stop{ false };

    void WorkerThread(ThreadData* data);
    bool StealTask(std::function<void()>& task);
};

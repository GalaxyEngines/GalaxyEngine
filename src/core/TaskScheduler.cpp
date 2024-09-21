// TaskScheduler.cpp

#include "TaskScheduler.h"

TaskScheduler::TaskScheduler(size_t threadCount) {
    for (size_t i = 0; i < threadCount; ++i) {
        auto data = std::make_unique<ThreadData>();
        data->worker = std::thread(&TaskScheduler::WorkerThread, this, data.get());
        threads.push_back(std::move(data));
    }
}

TaskScheduler::~TaskScheduler() {
    stop.store(true);
    for (auto& data : threads) {
        data->stopFlag = true;
        data->condition.notify_all();
    }

    for (auto& data : threads) {
        if (data->worker.joinable()) {
            data->worker.join();
        }
    }
}

template<typename Func, typename... Args>
auto TaskScheduler::SubmitTask(Func&& func, Args&&... args) -> std::future<decltype(func(args...))> {
    using ReturnType = decltype(func(args...));

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );
    std::future<ReturnType> result = task->get_future();

    size_t index = std::rand() % threads.size();
    {
        std::lock_guard<std::mutex> lock(threads[index]->queueMutex);
        threads[index]->taskQueue.push_back([task]() { (*task)(); });
    }
    threads[index]->condition.notify_one();

    return result;
}

void TaskScheduler::WorkerThread(ThreadData* data) {
    while (!data->stopFlag) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(data->queueMutex);
            data->condition.wait(lock, [data, this] {
                return data->stopFlag || !data->taskQueue.empty() || stop.load();
            });

            if (data->stopFlag && data->taskQueue.empty()) {
                return;
            }

            if (!data->taskQueue.empty()) {
                task = std::move(data->taskQueue.front());
                data->taskQueue.pop_front();
            }
        }

        if (task) {
            task();
        } else if (StealTask(task)) {
            task();
        }
    }
}

bool TaskScheduler::StealTask(std::function<void()>& task) {
    for (auto& otherThread : threads) {
        if (otherThread->stopFlag) {
            continue;
        }

        std::lock_guard<std::mutex> lock(otherThread->queueMutex);
        if (!otherThread->taskQueue.empty()) {
            task = std::move(otherThread->taskQueue.back());
            otherThread->taskQueue.pop_back();
            return true;
        }
    }
    return false;
}

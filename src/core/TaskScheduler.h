// #ifndef TASKSCHEDULERMODULE_H
// #define TASKSCHEDULERMODULE_H
//
// #include "ModuleInterface.h"
// #include <functional>
// #include <future>
// #include <queue>
// #include <thread>
// #include <vector>
// #include <mutex>
// #include <condition_variable>
// #include <atomic>
//
// class TaskSchedulerModule : public ModuleInterface {
// public:
//     TaskSchedulerModule();
//     ~TaskSchedulerModule() override;
//
//     // 模块初始化
//     void Initialize() override;
//
//     // 模块清理
//     void Cleanup() override;
//
//     // 事件处理
//     void OnEvent(const std::string& event) override;
//
//     // 处理传入的任务
//     void ProcessTask(const Task& task) override;
//
//     // 提供给外部模块的任务调度接口，返回一个 future 以便获取结果
//     template<typename Func, typename... Args>
//     auto ScheduleTask(Func&& func, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type>;
//
// private:
//     std::vector<std::thread> workers;
//     std::queue<std::function<void()>> tasks;
//     std::mutex queueMutex;
//     std::condition_variable cv;
//     std::atomic<bool> stop;
//
//     // 将任务放入队列
//     void EnqueueTask(std::function<void()> func);
//
//     // 工作线程函数
//     void WorkerThreadFunc();
//
//     // 停止任务调度器
//     void StopScheduler();
//
//     // 解析 JSON 格式的任务数据
//     std::string ParseTaskData(const std::string& data);
// };
//
// // 模板函数实现
// template<typename Func, typename... Args>
// auto TaskSchedulerModule::ScheduleTask(Func&& func, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type> {
//     using return_type = typename std::result_of<Func(Args...)>::type;
//
//     auto task = std::make_shared<std::packaged_task<return_type()>>(
//         std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
//     );
//
//     std::future<return_type> res = task->get_future();
//     {
//         std::lock_guard<std::mutex> lock(queueMutex);
//         tasks.emplace([task]() { (*task)(); });
//     }
//     cv.notify_one(); // 通知某个线程来处理任务
//
//     return res;
// }
//
// #endif // TASKSCHEDULERMODULE_H

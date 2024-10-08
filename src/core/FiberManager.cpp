// #include "ModuleInterface.h"
// #include <functional>
// #include <vector>
// #include <queue>
// #include <mutex>
// #include <condition_variable>
// #include <thread>
// #include <memory>
// #include <atomic>
// #include <iostream>
// #include <boost/context/fiber.hpp>
// #include <nlohmann/json.hpp> // 用于解析更好的数据格式（JSON）
//
// using json = nlohmann::json;
// using namespace MyEngine;
//
// class FiberManagerModule : public ModuleInterface {
// public:
//     FiberManagerModule() : stop(false) {}
//
//     void Initialize() override {
//         schedulerThread = std::thread(&FiberManagerModule::SchedulerThreadFunc, this);
//         std::cout << "FiberManagerModule Initialized." << std::endl;
//     }
//
//     void Cleanup() override {
//         {
//             std::lock_guard<std::mutex> lock(queueMutex);
//             stop = true;
//         }
//         cv.notify_all();
//         if (schedulerThread.joinable()) {
//             schedulerThread.join();
//         }
//         std::cout << "FiberManagerModule Cleaned up." << std::endl;
//     }
//
//     void OnEvent(const std::string& event) override {
//         std::cout << "FiberManager received event: " << event << std::endl;
//         if (event == "pause") {
//             PauseFibers();
//         }
//         else if (event == "resume") {
//             ResumeFibers();
//         }
//     }
//
//     void ProcessTask(const Task& task) override {
//         auto fiberFunc = ParseFiberTaskData(task.GetData());
//         EnqueueFiberTask(fiberFunc);
//     }
//
//     // 提供给UI的API接口
//     void RunFiber(std::function<void()> fiberFunc) {
//         EnqueueFiberTask(fiberFunc);
//     }
//
// private:
//     using Fiber = boost::context::fiber;
//
//     std::queue<std::function<void()>> fiberTasks;
//     std::mutex queueMutex;
//     std::condition_variable cv;
//     std::atomic<bool> stop;
//
//     std::thread schedulerThread;
//
//     void EnqueueFiberTask(std::function<void()> func) {
//         {
//             std::lock_guard<std::mutex> lock(queueMutex);
//             fiberTasks.push(func);
//         }
//         cv.notify_one();
//     }
//
//     void SchedulerThreadFunc() {
//         while (true) {
//             std::function<void()> taskFunc;
//             {
//                 std::unique_lock<std::mutex> lock(queueMutex);
//                 cv.wait(lock, [this]() { return stop.load() || !fiberTasks.empty(); });
//                 if (stop.load() && fiberTasks.empty()) {
//                     break;
//                 }
//                 taskFunc = fiberTasks.front();
//                 fiberTasks.pop();
//             }
//
//             // 启动一个 fiber
//             Fiber fiber = Fiber([taskFunc](Fiber&& sink) mutable {
//                 taskFunc();
//                 return std::move(sink);  // 切换回调度器 fiber
//                 });
//
//             // 执行并切换到 fiber
//             while (fiber) {
//                 fiber = std::move(fiber).resume();  // 使用 resume 来切换上下文
//             }
//         }
//     }
//
//     void PauseFibers() {
//         std::cout << "Pausing all fibers..." << std::endl;
//         // 实现暂停所有 fiber 的逻辑（如果需要）
//     }
//
//     void ResumeFibers() {
//         std::cout << "Resuming all fibers..." << std::endl;
//         // 实现恢复所有 fiber 的逻辑（如果需要）
//     }
//
//     std::function<void()> ParseFiberTaskData(const std::string& data) {
//         json parsedData = json::parse(data);
//         std::string action = parsedData["action"];
//         if (action == "complex_computation") {
//             return []() {
//                 std::cout << "Fiber complex computation..." << std::endl;
//                 // 在这里实现 fiber 的实际功能
//                 // Fiber 内部切换
//                 // 不使用 yield，fiber 会在完成后自动切换上下文
//                 };
//         }
//         return []() {
//             std::cout << "Running default fiber task..." << std::endl;
//             };
//     }
// };
//
// extern "C" ModuleInterface* CreateModule() {
//     return new FiberManagerModule();
// }

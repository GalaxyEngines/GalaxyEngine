#ifndef ASYNCLOADER_H
#define ASYNCLOADER_H

#include "ModuleInterface.h"  // 确保 AsyncLoader 继承 ModuleInterface
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

namespace GE {

    class AsyncLoader : public ModuleInterface {
    public:
        virtual ~AsyncLoader() = default;

        // 异步加载资源，资源加载后调用 callback
        virtual void LoadResourceAsync(const std::string& resourcePath, std::function<void(std::shared_ptr<std::vector<char>>)> callback) {
            std::cout << "Loading resource asynchronously from: " << resourcePath << std::endl;
            auto data = std::make_shared<std::vector<char>>(resourcePath.begin(), resourcePath.end());
            callback(data);
        }

        // 重写基类 ModuleInterface 的虚函数
        void initialize() override {
            std::cout << "AsyncLoader initialized." << std::endl;
        }

        void shutdown() override {
            std::cout << "AsyncLoader shutdown." << std::endl;
        }

        void onEvent(const std::string& event) override {
            std::cout << "AsyncLoader handling event: " << event << std::endl;
        }

        void processTask(const Task& task) override {
            std::cout << "AsyncLoader processing task: " << task.GetData() << std::endl;
        }

        // 可选的重载版本，用于直接处理字符串任务数据
        void processTask(const std::string& taskData) {
            std::cout << "AsyncLoader processing task data: " << taskData << std::endl;
        }
    };

}

#endif // ASYNCLOADER_H

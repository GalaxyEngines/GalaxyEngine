#ifndef ASYNCLOADER_H
#define ASYNCLOADER_H

#include "ModuleInterface.h"  // 确保 AsyncLoader 继承 ModuleInterface
#include <functional>
#include <string>
#include <memory>
#include <vector>

namespace MyEngine {

    class AsyncLoader : public ModuleInterface {
    public:
        virtual ~AsyncLoader() = default;

        // 异步加载资源，资源加载后调用 callback
        virtual void LoadResourceAsync(const std::string& resourcePath, std::function<void(std::shared_ptr<std::vector<char>>)> callback) = 0;

        // 重写基类 ModuleInterface 的虚函数
        virtual void initialize() override = 0;
        virtual void shutdown() override = 0;
        virtual void onEvent(const std::string& event) override = 0;
        virtual void processTask(const std::string& taskData) override = 0;
    };

}

#endif // ASYNCLOADER_H

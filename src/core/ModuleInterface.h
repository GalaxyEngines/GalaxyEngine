#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <string>
#include <iostream>

namespace GE {


    class ModuleInterface {
    public:

        ModuleInterface();
        virtual ~ModuleInterface();


        virtual void initialize();
        virtual void shutdown();


        virtual void onEvent(const std::string& event);


        virtual void processTask(const class Task& task);


        virtual void update();

    protected:
        bool initialized;
        bool active;
    };

    // 任务类
    class Task {
    public:
        enum class TaskType { LoadResource, Compute, renderFrame };

        Task(const std::string& data, TaskType type) : data_(data), type_(type) {}

        const std::string& GetData() const { return data_; }
        TaskType GetType() const { return type_; }

    private:
        std::string data_;
        TaskType type_;
    };

}

#endif // MODULE_INTERFACE_H

一款轻量好用的游戏引擎
![Image text](https://github.com/GalaxyEngines/GalaxyEngine/blob/main/LOGO/logo.jpeg)

设计 
RENDERER
VulkanMemoryManager是vulkan的显存管理器
StreamingManager是资源流管理，来动态加载纹理和模型
TextureCompressor纹理压缩和解压缩
CORE
TaskScheduler多线程任务
FiberManager纤
AsyncLoader异步加载

这是模型管理器
// ModuleInterface.h

#pragma once
#include <string>

class ModuleInterface {
public:
    virtual ~ModuleInterface() = default;
    virtual void Initialize() = 0;
    virtual void Cleanup() = 0;
    virtual void OnEvent(const std::string& event) = 0; // 接口
};

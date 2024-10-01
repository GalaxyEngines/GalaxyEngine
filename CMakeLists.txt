cmake_minimum_required(VERSION 3.10)
project(GalaxyEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖库
find_package(Boost REQUIRED COMPONENTS context)
find_package(Vulkan REQUIRED)
find_package(nlohmann_json REQUIRED)

# 包含头文件目录
include_directories(include)
include_directories(thirdparty/imgui)

# 添加子目录的外部库
add_subdirectory(thirdparty/GLFW)
add_subdirectory(thirdparty/glm)

# 手动管理源文件
set(SOURCE_FILES
    src/core/AsyncLoader.cpp
    src/core/FiberManager.cpp
    src/core/MemoryManager.cpp
    src/core/TaskScheduler.cpp
    src/graphics/VulkanBase.cpp
    src/platform/a.cpp
    src/user/a.cpp
)

# 创建主可执行文件
add_executable(GalaxyEngine ${SOURCE_FILES})

# 包含第三方库的头文件和库链接
target_include_directories(GalaxyEngine PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
    thirdparty/glfw/include
    thirdparty/glm
    thirdparty/imgui
)

target_link_libraries(GalaxyEngine PRIVATE
    ${Boost_LIBRARIES}
    ${Vulkan_LIBRARIES}
    nlohmann_json::nlohmann_json
    glfw
    glm
)

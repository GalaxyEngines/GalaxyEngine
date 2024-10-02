cmake_minimum_required(VERSION 3.10)

# 项目定义
project(GalaxyEngine LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 手动设置 Windows SDK 版本
set(CMAKE_SYSTEM_VERSION 10.0.22621.0)

# 手动设置vcpkg路径
set(VCPKG_ROOT "G:/vcpkg")

# 包含vcpkg管理的CMake工具链文件
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# 查找Boost库
find_package(Boost REQUIRED COMPONENTS context)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
else()
    message(FATAL_ERROR "Could not find Boost! Please check your Boost installation.")
endif()

# 查找nlohmann_json库
find_package(nlohmann_json CONFIG REQUIRED)

# 查找GLFW库，优先使用vcpkg中的GLFW
find_package(glfw3 CONFIG REQUIRED)# 手动设置Vulkan路径
set(VULKAN_SDK "C:/VulkanSDK/1.3.290.0")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/Include")
set(Vulkan_LIBRARY "${VULKAN_SDK}/Lib/vulkan-1.lib")

# 包含Vulkan头文件
include_directories(${Vulkan_INCLUDE_DIR})

# 手动管理源文件
set(SOURCE_FILES
    src/core/AsyncLoader.cpp
    src/core/FiberManager.cpp
    src/core/MemoryManager.cpp
    src/core/TaskScheduler.cpp
    src/core/VulkanBase.cpp
    src/platform/a.cpp
    src/user/a.cpp
)

# 创建主可执行文件
add_executable(GalaxyEngine ${SOURCE_FILES})

# 包含第三方库的头文件
target_include_directories(GalaxyEngine PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIR}
    thirdparty/imgui
)

# 链接库到可执行文件
target_link_libraries(GalaxyEngine PRIVATE
    ${Boost_LIBRARIES}
    ${Vulkan_LIBRARY}
    nlohmann_json::nlohmann_json  # 链接nlohmann_json库
    glfw  # 通过vcpkg链接GLFW
    glm
)

# 提示用户设置Windows SDK版本
message(STATUS "Ensure you are targeting the correct Windows SDK in Visual Studio (e.g., Windows 10).")

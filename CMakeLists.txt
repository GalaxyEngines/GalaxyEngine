cmake_minimum_required(VERSION 3.30)

# vcpkg工具链
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 项目设置
project(GalaxyEngine LANGUAGES CXX)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加所有源文件
file(GLOB_RECURSE SOURCE src/*.cpp src/*.h)
if(NOT SOURCE)
    message(FATAL_ERROR "No source files found in src directory.")
endif()

# 设置可执行文件
add_executable(GalaxyEngine ${SOURCE})

# 链接所需库
find_package(glfw3 CONFIG REQUIRED)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "glfw3 not found. Please install it via vcpkg.")
endif()

find_package(glm CONFIG REQUIRED)
if(NOT glm_FOUND)
    message(FATAL_ERROR "glm not found. Please install it via vcpkg.")
endif()

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found. Please install it via vcpkg.")
endif()

find_package(imgui CONFIG REQUIRED)
if(NOT imgui_FOUND)
    message(FATAL_ERROR "imgui not found. Please install it via vcpkg.")
endif()

target_link_libraries(GalaxyEngine PRIVATE
        glfw
        glm::glm-header-only
        Vulkan::Vulkan
        imgui::imgui
)

# 设置头文件目录
target_include_directories(GalaxyEngine PRIVATE include)

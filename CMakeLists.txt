cmake_minimum_required(VERSION 3.30)
# 设置VCPKG路径
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 项目信息
project(GalaxyEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 查找所有核心源文件
file(GLOB_RECURSE CORE_SOURCE src/*.cpp)

if(NOT CORE_SOURCE)
    message(FATAL_ERROR "No core source files found in src/core directory.")
endif()


# 设置资源目录
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
add_definitions(-DRESOURCE_PATH="${RESOURCE_DIR}")

# 创建可执行文件，将核心文件和 UI 文件添加到目标源中
add_executable(GalaxyEngine ${CORE_SOURCE})

# 添加资源复制命令
add_custom_command(
        TARGET GalaxyEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR} $<TARGET_FILE_DIR:GalaxyEngine>/resources
)

# 查找依赖项
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

# 包含 imgui 实现文件路径
include_directories(${vcpkg_INCLUDE_DIRS}/imgui/backends)

# 链接库
target_link_libraries(GalaxyEngine PRIVATE
        glfw
        glm::glm-header-only
        Vulkan::Vulkan
        imgui::imgui
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
        OpenAL::OpenAL
        ${BULLET_LIBRARIES}
)
# 添加 include 目录路径
target_include_directories(GalaxyEngine PRIVATE include)
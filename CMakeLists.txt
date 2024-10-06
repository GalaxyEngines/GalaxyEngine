cmake_minimum_required(VERSION 3.30)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 项目定义
project(GalaxyEngine LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE
        "src/core/*.cpp"
        "src/engine/*.cpp"
        "src/platform/*.cpp"
        "src/ui/*.cpp"
        "src/user/*.cpp"
)
add_executable(GalaxyEngine ${SOURCE})

find_package(glfw3 REQUIRED)
target_link_libraries(GalaxyEngine PRIVATE glfw)
find_package(glm REQUIRED)
target_link_libraries(GalaxyEngine PRIVATE glm::glm-header-only)
find_package(Vulkan REQUIRED)
target_link_libraries(GalaxyEngine PRIVATE Vulkan::Vulkan)

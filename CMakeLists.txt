cmake_minimum_required(VERSION 3.30)
# 设置VCPKG路径
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# 项目信息
project(GalaxyEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/GalaxyEngine)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/GalaxyEngine/lib)

# 查找所有核心源文件
file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h)

# 设置资源目录
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
add_definitions(-DRESOURCE_PATH="${RESOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
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
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS context)


add_subdirectory(lib/GEui)
add_subdirectory(lib/GERender)
add_subdirectory(lib/vk-bootstrap)
# 链接库
target_link_libraries(GalaxyEngine PRIVATE
        glfw
        glm::glm-header-only
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
        OpenAL::OpenAL
        ${BULLET_LIBRARIES}
        Boost::context
        GEUi
        GERender
        vk-bootstrap::vk-bootstrap
)
# 添加 include 目录路径
target_include_directories(GalaxyEngine PRIVATE lib/GEui/include)
target_include_directories(GalaxyEngine PRIVATE lib/GERender/include)

target_include_directories(GalaxyEngine PRIVATE src)
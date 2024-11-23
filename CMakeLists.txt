cmake_minimum_required(VERSION 3.30)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(GalaxyEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/GalaxyEngine)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/GalaxyEngine/lib)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h include/*.h)

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
add_definitions(-DRESOURCE_PATH="${RESOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_executable(GalaxyEngine ${CORE_SOURCE})

add_custom_command(
        TARGET GalaxyEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR} $<TARGET_FILE_DIR:GalaxyEngine>/resources
)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS context)


add_subdirectory(lib/vk-bootstrap)

target_link_libraries(GalaxyEngine PRIVATE
        glfw
        glm::glm-header-only
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
        OpenAL::OpenAL
        ${BULLET_LIBRARIES}
        Boost::context
        vk-bootstrap::vk-bootstrap
)

target_include_directories(GalaxyEngine PRIVATE include)
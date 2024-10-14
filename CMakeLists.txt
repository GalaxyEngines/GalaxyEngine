cmake_minimum_required(VERSION 3.30)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(GalaxyEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



file(GLOB_RECURSE SOURCE src/*.cpp src/*.h)
if(NOT SOURCE)
    message(FATAL_ERROR "No source files found in src directory.")
endif()

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
add_definitions(-DRESOURCE_PATH="${RESOURCE_DIR}")

add_executable(GalaxyEngine ${SOURCE})
add_custom_command(
        TARGET GalaxyEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR} $<TARGET_FILE_DIR:GalaxyEngine>/resources
)


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
find_package(nlohmann_json CONFIG REQUIRED)
if(NOT nlohmann_json_FOUND)
    message(FATAL_ERROR "nlohmann_json not found. Please install it via vcpkg.")
endif()
find_package(yaml-cpp CONFIG REQUIRED)
if(NOT yaml-cpp_FOUND)
    message(FATAL_ERROR "yaml-cpp not found. Please install it via vcpkg.")
endif()


target_link_libraries(GalaxyEngine PRIVATE
        glfw
        glm::glm-header-only
        Vulkan::Vulkan
        imgui::imgui
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
)
target_include_directories(GalaxyEngine PRIVATE include)
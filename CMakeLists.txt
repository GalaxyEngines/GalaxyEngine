cmake_minimum_required(VERSION 3.10 )
project(GalaxyEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE src ./src/*.cpp)
include_directories(include)
include_directories(thirdparty/imgui)
add_subdirectory(test)
add_subdirectory(thirdparty/GLFW)
add_subdirectory(thirdparty/glm)

add_executable(GalaxyEngine ${src}
        src/platform/a.cpp
        src/user/a.cpp)
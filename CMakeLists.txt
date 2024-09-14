cmake_minimum_required(VERSION 3.10 )
project(GalaxyEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


aux_source_directory(src soure)
include_directories(include)

add_subdirectory(test)
add_subdirectory(thirdparty/GLFW)
add_subdirectory(thirdparty/glm)

add_executable(GalaxyEngine ${soure})
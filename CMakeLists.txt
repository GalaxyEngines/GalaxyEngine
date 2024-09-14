cmake_minimum_required(VERSION 3.10)
project(GalaxyEngine)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(test)

add_executable(GalaxyEngine src/main.cpp)

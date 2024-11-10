cmake_minimum_required(VERSION 3.30)
project(GEUi)

project(GEUi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h)

add_library(GEUi SHARED ${CORE_SOURCE})

project(GEUi)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h)

add_library(GEUi SHARED ${CORE_SOURCE}
        include/a.cpp)

find_package(imgui CONFIG REQUIRED)

target_link_libraries(GEUi PRIVATE imgui::imgui)
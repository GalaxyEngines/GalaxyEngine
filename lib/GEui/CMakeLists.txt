project(GEUi)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h)

add_library(GEUi SHARED ${CORE_SOURCE})

find_package(imgui CONFIG REQUIRED)

target_link_libraries(GEUi PRIVATE imgui::imgui)

target_include_directories(GEUi PRIVATE include)

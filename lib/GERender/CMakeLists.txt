project(GERender)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h)

add_library(GERender SHARED ${CORE_SOURCE})

find_package(Vulkan REQUIRED)

target_link_libraries(GERender PRIVATE Vulkan::Vulkan)
target_link_libraries(GERender PRIVATE vk-bootstrap::vk-bootstrap)

target_include_directories(GERender PRIVATE include)

